#include "claves.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

//#define DEBUG 0

#ifdef DEBUG
#define log(...) printf(__VA_ARGS__)
#else
#define log(...)
#endif

void* manejar_cliente(void *arg) {
    int sock = *((int*)arg);
    free(arg);

    uint8_t opcode;
    if (recv(sock, &opcode, 1, 0) != 1) {
        close(sock);
        return NULL;
    }

    char buffer[1024];
    int32_t respuesta;

    switch (opcode) {
        case 0: { // destroy
            respuesta = destroy();
            break;
        }
        case 1: { // set_value
            uint32_t key_net;
            uint8_t len, N;
            char value1[256];
            double V_value2[32];
            uint32_t x_net, y_net;

            recv(sock, buffer, 1024, 0);
            size_t offset = 0;

            memcpy(&key_net, buffer + offset, 4); offset += 4;
            memcpy(&len, buffer + offset, 1); offset += 1;
            memcpy(value1, buffer + offset, len); offset += len;
            value1[len] = '\0';
            memcpy(&N, buffer + offset, 1); offset += 1;
            memcpy(V_value2, buffer + offset, N * sizeof(double)); offset += N * sizeof(double);
            memcpy(&x_net, buffer + offset, 4); offset += 4;
            memcpy(&y_net, buffer + offset, 4); offset += 4;

            struct Coord coord = {ntohl(x_net), ntohl(y_net)};
            respuesta = set_value(ntohl(key_net), value1, N, V_value2, coord);
            break;
        }
        case 2: { // get_value
            uint32_t key_net;
            recv(sock, &key_net, 4, 0);
            int key = ntohl(key_net);

            char value1[256] = {0};
            int N_value2;
            double V_value2[32];
            struct Coord value3;
            int res = get_value(key, value1, &N_value2, V_value2, &value3);
            int32_t res_net = htonl(res);
            send(sock, &res_net, 4, 0);
            if (res == 0) {
                uint8_t len = strlen(value1);
                send(sock, &len, 1, 0);
                send(sock, value1, len, 0);
                uint8_t N = N_value2;
                send(sock, &N, 1, 0);
                send(sock, V_value2, N_value2 * sizeof(double), 0);
                uint32_t x_net = htonl(value3.x);
                uint32_t y_net = htonl(value3.y);
                send(sock, &x_net, 4, 0);
                send(sock, &y_net, 4, 0);
            }
            break;
        }
        case 3: { // modify_value
            uint32_t key_net;
            recv(sock, &key_net, 4, 0);
            int key = ntohl(key_net);

            uint8_t len;
            recv(sock, &len, 1, 0);
            char value1[256] = {0};
            recv(sock, value1, len, 0);
            value1[len] = '\0';

            uint8_t N_net;
            recv(sock, &N_net, 1, 0);
            int N = N_net;

            double V_value2[32];
            recv(sock, V_value2, N * sizeof(double), 0);

            uint32_t x_net, y_net;
            recv(sock, &x_net, 4, 0);
            recv(sock, &y_net, 4, 0);
            struct Coord value3 = {ntohl(x_net), ntohl(y_net)};

            int res = modify_value(key, value1, N, V_value2, value3);
            int32_t res_net = htonl(res);
            send(sock, &res_net, 4, 0);
            break;
        }
        case 4: { // delete_key
            uint32_t key_net;
            recv(sock, &key_net, 4, 0);
            int key = ntohl(key_net);
            int res = delete_key(key);
            int32_t res_net = htonl(res);
            send(sock, &res_net, 4, 0);
            break;
        }
        case 5: { // exist
            uint32_t key_net;
            recv(sock, &key_net, 4, 0);
            int key = ntohl(key_net);
            int res = exist(key);
            int32_t res_net = htonl(res);
            send(sock, &res_net, 4, 0);
            break;
        }
        default: {
            // Código desconocido
            int32_t res_net = htonl(-1);
            send(sock, &res_net, 4, 0);
            break;
        }
    }

    respuesta = htonl(respuesta);
    send(sock, &respuesta, 4, 0);
    close(sock);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Error en bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("Error en listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int *client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, NULL, NULL);

        pthread_t thread;
        pthread_create(&thread, NULL, manejar_cliente, client_fd);
        pthread_detach(thread);
    }

    return 0;
} 

