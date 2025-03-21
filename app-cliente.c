#include <stdio.h>
#include "claves.h"

int main() {
    int key = 42;
    char value1[256] = "Ejemplo de cadena";
    double V_value2[3] = {1.1, 2.2, 3.3};
    struct Coord value3 = {10, 20};
    int N_value2 = 3;

    printf("Cl0: Iniciando cliente...\n");

    // Insertar una tupla
    if (set_value(key, value1, N_value2, V_value2, value3) == 0) {
        printf("Cl0: Tupla insertada correctamente.\n");
    } else {
        printf("Cl0: Error al insertar la tupla.\n");
    }

    // Obtener la tupla
    char retrieved_value1[256];
    double retrieved_V_value2[32];
    int retrieved_N_value2;
    struct Coord retrieved_value3;

    if (get_value(key, retrieved_value1, &retrieved_N_value2, retrieved_V_value2, &retrieved_value3) == 0) {
        printf("Cl0: Tupla obtenida correctamente: %s, [%d valores], (%d, %d)\n",
               retrieved_value1, retrieved_N_value2, retrieved_value3.x, retrieved_value3.y);
    } else {
        printf("Cl0: Error al obtener la tupla.\n");
    }

    // Modificar la tupla
    char new_value1[256] = "Nuevo valor de cadena";
    double new_V_value2[2] = {9.9, 8.8};
    struct Coord new_value3 = {30, 40};
    int new_N_value2 = 2;

    if (modify_value(key, new_value1, new_N_value2, new_V_value2, new_value3) == 0) {
        printf("Cl0: Tupla modificada correctamente.\n");
    } else {
        printf("Cl0: Error al modificar la tupla.\n");
    }

    // Verificar existencia
    if (exist(key) == 1) {
        printf("Cl0: La clave %d existe.\n", key);
    } else {
        printf("Cl0: La clave %d no existe.\n", key);
    }

    // Borrar la tupla
    if (delete_key(key) == 0) {
        printf("Cl0: Tupla eliminada correctamente.\n");
    } else {
        printf("Cl0: Error al eliminar la tupla.\n");
    }

    printf("Cl0: Cliente finalizado correctamente.\n");
    return 0;
}
