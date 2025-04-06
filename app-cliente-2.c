#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"

int main() {
    if (init() < 0) {
        printf("Error al iniciar la conexión con el servidor\n");
        return -1;
    }

    char key[] = "clave1";
    char value1[256] = "valor original";
    int value2 = 10;
    double value3 = 3.14;

    // Insertar clave
    if (set_value(key, value1, value2, value3) == 0) {
        printf("Clave '%s' insertada correctamente.\n", key);
    } else {
        printf("Error al insertar clave '%s'.\n", key);
    }

    // Modificar clave
    strcpy(value1, "valor modificado");
    value2 = 99;
    value3 = 42.42;

    if (modify_value(key, value1, value2, value3) == 0) {
        printf("Clave '%s' modificada correctamente.\n", key);
    } else {
        printf("Error al modificar clave '%s'.\n", key);
    }

    // Obtener valor de la clave
    char recv_value1[256];
    int recv_value2;
    double recv_value3;

    if (get_value(key, recv_value1, &recv_value2, &recv_value3) == 0) {
        printf("Valores obtenidos de '%s': %s, %d, %.2f\n", key, recv_value1, recv_value2, recv_value3);
    } else {
        printf("Error al obtener valores de la clave '%s'.\n", key);
    }

    // Eliminar clave
    if (delete_key(key) == 0) {
        printf("Clave '%s' eliminada correctamente.\n", key);
    } else {
        printf("Error al eliminar clave '%s'.\n", key);
    }

    // Verificar existencia
    int existe = exist(key);
    if (existe == 1) {
        printf("La clave '%s' aún existe.\n", key);
    } else if (existe == 0) {
        printf("La clave '%s' no existe (correcto).\n", key);
    } else {
        printf("Error al verificar existencia de la clave '%s'.\n", key);
    }

    if (destroy() < 0) {
        printf("Error al cerrar la conexión con el servidor\n");
        return -1;
    }

    return 0;
}
