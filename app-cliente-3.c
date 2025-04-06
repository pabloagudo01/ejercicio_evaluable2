#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"

int main() {
    int key = 110;
    char value1[256] = "nuevo valor";
    double V_value2[3] = {2.34, 5.67, 8.90};
    int N_value2 = 3;
    struct Coord value3 = {15, 25};

    printf("Cl3: Iniciando cliente...\n");

    // Intentar obtener valor antes de crear la clave
    char recv_value1[256];
    double recv_V_value2[32];
    int recv_N_value2;
    struct Coord recv_value3;

    if (get_value(key, recv_value1, &recv_N_value2, recv_V_value2, &recv_value3) == 0) {
        printf("Cl3: Valores obtenidos de '%d': %s, [%d valores], (%d, %d)\n", key,
               recv_value1, recv_N_value2, recv_value3.x, recv_value3.y);
    } else {
        printf("Cl3: Error al obtener valores de la clave '%d' (esperado).\n", key);
    }

    // Crear clave
    if (set_value(key, value1, N_value2, V_value2, value3) == 0) {
        printf("Cl3: Clave '%d' creada correctamente.\n", key);
    } else {
        printf("Cl3: Error al crear clave '%d'.\n", key);
    }

    // Obtener valor después de crear la clave
    if (get_value(key, recv_value1, &recv_N_value2, recv_V_value2, &recv_value3) == 0) {
        printf("Cl3: Valores obtenidos de '%d': %s, [%d valores], (%d, %d)\n", key,
               recv_value1, recv_N_value2, recv_value3.x, recv_value3.y);
    } else {
        printf("Cl3: Error al obtener valores de la clave '%d'.\n", key);
    }

    // Eliminar clave
    if (delete_key(key) == 0) {
        printf("Cl3: Clave '%d' eliminada correctamente.\n", key);
    } else {
        printf("Cl3: Error al eliminar clave '%d'.\n", key);
    }

    printf("Cl3: Cliente finalizado correctamente.\n");
    return 0;
}