#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"

int main() {
    int key = 101;
    char value1[256] = "valor original";
    double V_value2[3] = {1.23, 4.56, 7.89};
    int N_value2 = 3;
    struct Coord value3 = {5, 10};

    printf("Cl1: Iniciando cliente...\n");

    // Insertar clave
    if (set_value(key, value1, N_value2, V_value2, value3) == 0) {
        printf("Cl1: Clave '%d' insertada correctamente.\n", key);
    } else {
        printf("Cl1: Error al insertar clave '%d'.\n", key);
    }

    // Modificar clave
    char new_value1[256] = "valor modificado";
    double new_V_value2[2] = {9.9, 8.8};
    int new_N_value2 = 2;
    struct Coord new_value3 = {20, 30};

    if (modify_value(key, new_value1, new_N_value2, new_V_value2, new_value3) == 0) {
        printf("Cl1: Clave '%d' modificada correctamente.\n", key);
    } else {
        printf("Cl1: Error al modificar clave '%d'.\n", key);
    }

    // Obtener valor
    char recv_value1[256];
    double recv_V_value2[32];
    int recv_N_value2;
    struct Coord recv_value3;

    if (get_value(key, recv_value1, &recv_N_value2, recv_V_value2, &recv_value3) == 0) {
        printf("Cl1: Valores obtenidos de '%d': %s, [%d valores], (%d, %d)\n", key,
               recv_value1, recv_N_value2, recv_value3.x, recv_value3.y);
    } else {
        printf("Cl2: Error al obtener valores de la clave '%d'.\n", key);
    }

    // Eliminar clave
    if (delete_key(key) == 0) {
        printf("Cl1: Clave '%d' eliminada correctamente.\n", key);
    } else {
        printf("Cl1: Error al eliminar clave '%d'.\n", key);
    }

    // Verificar existencia
    int existe = exist(key);
    if (existe == 1) {
        printf("Cl1: La clave '%d' aún existe.\n", key);
    } else if (existe == 0) {
        printf("Cl1: La clave '%d' no existe (correcto).\n", key);
    } else {
        printf("Cl1: Error al verificar existencia de la clave '%d'.\n", key);
    }

    printf("Cl1: Cliente finalizado correctamente.\n");
    return 0;
}
