#include "claves.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdint.h>
#include <sys/uio.h>

//#define DEBUG 0  // Descomentar para depuración

#ifdef DEBUG
#define log(...) printf(__VA_ARGS__)
#else
#define log(...)
#endif

static int conectar_servidor() {
    char *ip = getenv("IP_TUPLAS");
    char *port_str = getenv("PORT_TUPLAS");
    if (!ip || !port_str) {
        log("Error: Variables IP_TUPLAS o PORT_TUPLAS no definidas\n");
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creando socket");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port_str));
    
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("Dirección IP inválida");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr))) {
        perror("Error de conexión");
        close(sock);
        return -1;
    }

    return sock;
}

// ---------------------- Serialización y envío de datos ----------------------

int destroy() {
    log("[PID %d] destroy llamada\n", getpid());

    int sock = conectar_servidor();
    if (sock == -1) return -2;

    uint8_t opcode = 0;
    if (send(sock, &opcode, 1, 0) != 1) {
        close(sock);
        return -2;
    }

    int32_t respuesta;
    if (recv(sock, &respuesta, 4, 0) != 4) {
        close(sock);
        return -2;
    }

    close(sock);
    log("[PID %d] destroy terminada con respuesta: %d\n", getpid(), ntohl(respuesta));
    return ntohl(respuesta);
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    log("[PID %d] set_value llamada con clave %d\n", getpid(), key);

    if (strlen(value1) > 255 || N_value2 < 1 || N_value2 > 32) return -1;

    int sock = conectar_servidor();
    if (sock == -1) return -2;

    // Serializar datos en buffer
    uint8_t opcode = 1;
    uint32_t key_net = htonl(key);
    uint8_t len = strlen(value1);
    uint8_t N_net = N_value2;
    uint32_t x_net = htonl(value3.x);
    uint32_t y_net = htonl(value3.y);

    log("[PID %d] Enviando datos: key=%d, value1=%s, N_value2=%d\n", getpid(), key, value1, N_value2);

    char buffer[1024];
    size_t offset = 0;
    memcpy(buffer + offset, &opcode, 1); offset += 1;
    memcpy(buffer + offset, &key_net, 4); offset += 4;
    memcpy(buffer + offset, &len, 1); offset += 1;
    memcpy(buffer + offset, value1, len); offset += len;
    memcpy(buffer + offset, &N_net, 1); offset += 1;
    memcpy(buffer + offset, V_value2, N_value2 * sizeof(double)); offset += N_value2 * sizeof(double);
    memcpy(buffer + offset, &x_net, 4); offset += 4;
    memcpy(buffer + offset, &y_net, 4); offset += 4;

    if (send(sock, buffer, offset, 0) != (ssize_t)offset) {
        close(sock);
        return -2;
    }

    int32_t respuesta;
    if (recv(sock, &respuesta, 4, 0) != 4) {
        close(sock);
        return -2;
    }

    close(sock);
    log("[PID %d] set_value terminada con respuesta: %d\n", getpid(), ntohl(respuesta));
    return ntohl(respuesta);
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    log("[PID %d] get_value llamada con clave %d\n", getpid(), key);

    int sock = conectar_servidor();
    if (sock == -1) return -2;

    // Serializar solicitud
    uint8_t opcode = 2;
    uint32_t key_net = htonl(key);
    char buffer[5];
    memcpy(buffer, &opcode, 1);
    memcpy(buffer + 1, &key_net, 4);

    log("[PID %d] Enviando solicitud para obtener valor de clave %d\n", getpid(), key);

    if (send(sock, buffer, 5, 0) != 5) {
        close(sock);
        return -2;
    }

    // Recibir respuesta
    int32_t respuesta;
    if (recv(sock, &respuesta, 4, 0) != 4) {
        close(sock);
        return -2;
    }

    respuesta = ntohl(respuesta);
    if (respuesta != 0) {
        close(sock);
        return respuesta;
    }

    uint8_t len;
    if (recv(sock, &len, 1, 0) != 1 || recv(sock, value1, len, 0) != len) {
        close(sock);
        return -2;
    }
    value1[len] = '\0';

    uint8_t N;
    if (recv(sock, &N, 1, 0) != 1 || recv(sock, V_value2, N * sizeof(double), 0) != (ssize_t)(N * sizeof(double))) {
        close(sock);
        return -2;
    }
    *N_value2 = N;

    uint32_t x_net, y_net;
    if (recv(sock, &x_net, 4, 0) != 4 || recv(sock, &y_net, 4, 0) != 4) {
        close(sock);
        return -2;
    }

    value3->x = ntohl(x_net);
    value3->y = ntohl(y_net);

    close(sock);
    log("[PID %d] get_value terminada para clave %d\n", getpid(), key);
    return 0;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    log("[PID %d] modify_value llamada con clave %d\n", getpid(), key);

    if (strlen(value1) > 255 || N_value2 < 1 || N_value2 > 32) return -1;

    int sock = conectar_servidor();
    if (sock == -1) return -2;

    uint8_t opcode = 3;
    uint32_t key_net = htonl(key);
    uint8_t len = strlen(value1);
    uint8_t N_net = N_value2;
    uint32_t x_net = htonl(value3.x);
    uint32_t y_net = htonl(value3.y);

    struct iovec iov[8] = {
        {&opcode, 1},
        {&key_net, 4},
        {&len, 1},
        {value1, len},
        {&N_net, 1},
        {V_value2, N_value2 * sizeof(double)},
        {&x_net, 4},
        {&y_net, 4}
    };

    log("[PID %d] Enviando datos modificados para la clave %d\n", getpid(), key);

    if (writev(sock, iov, 8) == -1) {
        close(sock);
        return -2;
    }

    int32_t respuesta;
    if (recv(sock, &respuesta, 4, 0) != 4) {
        close(sock);
        return -2;
    }

    close(sock);
    log("[PID %d] modify_value terminada con respuesta: %d\n", getpid(), ntohl(respuesta));
    return ntohl(respuesta);
}

int delete_key(int key) {
    log("[PID %d] delete_key llamada con clave %d\n", getpid(), key);

    int sock = conectar_servidor();
    if (sock == -1) return -2;

    uint8_t opcode = 4;
    uint32_t key_net = htonl(key);

    if (send(sock, &opcode, 1, 0) != 1 || send(sock, &key_net, 4, 0) != 4) {
        close(sock);
        return -2;
    }

    int32_t respuesta;
    if (recv(sock, &respuesta, 4, 0) != 4) {
        close(sock);
        return -2;
    }

    close(sock);
    log("[PID %d] delete_key terminada con respuesta: %d\n", getpid(), ntohl(respuesta));
    return ntohl(respuesta);
}
