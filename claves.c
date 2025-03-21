#include "claves.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_KEY 1000  // Tamaño arbitrario para la tabla hash

typedef struct Tupla {
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
    struct Tupla *next; // Para manejar colisiones en la tabla hash
} Tupla;

static Tupla *tabla[MAX_KEY];

// Declaramos un mutex global para proteger la tabla
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// **Función hash para distribuir claves en la tabla**
int hash(int key) {
    return key % MAX_KEY;
}

// **Elimina todas las tuplas de la tabla**
int destroy(void) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_KEY; i++) {
        Tupla *actual = tabla[i];
        while (actual) {
            Tupla *temp = actual;
            actual = actual->next;
            free(temp);
        }
        tabla[i] = NULL;
    }
    pthread_mutex_unlock(&mutex);
    printf("Cl: Todas las claves han sido eliminadas.\n");
    return 0;
}

// **Inserta una nueva tupla en la tabla**
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (N_value2 < 1 || N_value2 > 32 || strlen(value1) > 255) {
        printf("Cl: Error - Parámetros inválidos para clave %d.\n", key);
        return -1;
    }

    pthread_mutex_lock(&mutex);
    int index = hash(key);
    Tupla *actual = tabla[index];

    while (actual) {
        if (actual->key == key) {
            pthread_mutex_unlock(&mutex);
            printf("Cl: Error - La clave %d ya existe.\n", key);
            return -1;
        }
        actual = actual->next;
    }

    Tupla *nueva = (Tupla *)malloc(sizeof(Tupla));
    if (!nueva) {
        pthread_mutex_unlock(&mutex);
        printf("Cl: Error - No se pudo asignar memoria para clave %d.\n", key);
        return -1;
    }

    nueva->key = key;
    strcpy(nueva->value1, value1);
    nueva->N_value2 = N_value2;
    memcpy(nueva->V_value2, V_value2, N_value2 * sizeof(double));
    nueva->value3 = value3;
    nueva->next = tabla[index];
    tabla[index] = nueva;

    pthread_mutex_unlock(&mutex);
    printf("Cl: Clave %d insertada correctamente.\n", key);
    return 0;
}

// **Obtiene una tupla si existe**
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    pthread_mutex_lock(&mutex);
    int index = hash(key);
    Tupla *actual = tabla[index];

    while (actual) {
        if (actual->key == key) {
            strcpy(value1, actual->value1);
            *N_value2 = actual->N_value2;
            memcpy(V_value2, actual->V_value2, actual->N_value2 * sizeof(double));
            *value3 = actual->value3;
            pthread_mutex_unlock(&mutex);
            printf("Cl: Clave %d obtenida correctamente.\n", key);
            return 0;
        }
        actual = actual->next;
    }

    pthread_mutex_unlock(&mutex);
    printf("Cl: Error - Clave %d no encontrada.\n", key);
    return -1;
}

// **Modifica una tupla existente**
int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (N_value2 < 1 || N_value2 > 32 || strlen(value1) > 255) {
        printf("Cl: Error - Parámetros inválidos para modificar clave %d.\n", key);
        return -1;
    }

    pthread_mutex_lock(&mutex);
    int index = hash(key);
    Tupla *actual = tabla[index];

    while (actual) {
        if (actual->key == key) {
            strcpy(actual->value1, value1);
            actual->N_value2 = N_value2;
            memcpy(actual->V_value2, V_value2, N_value2 * sizeof(double));
            actual->value3 = value3;
            pthread_mutex_unlock(&mutex);
            printf("Cl: Clave %d modificada correctamente.\n", key);
            return 0;
        }
        actual = actual->next;
    }

    pthread_mutex_unlock(&mutex);
    printf("Cl: Error - Clave %d no encontrada.\n", key);
    return -1;
}

// **Elimina una tupla si existe**
int delete_key(int key) {
    printf("Cl: Intentando eliminar la clave %d...\n", key);
    pthread_mutex_lock(&mutex);

    int index = hash(key);
    Tupla *actual = tabla[index];
    Tupla *prev = NULL;

    while (actual) {
        if (actual->key == key) {
            if (prev == NULL) {
                tabla[index] = actual->next;
            } else {
                prev->next = actual->next;
            }

            free(actual);
            pthread_mutex_unlock(&mutex);
            printf("Cl: Clave %d eliminada correctamente.\n", key);
            return 0;
        }
        prev = actual;
        actual = actual->next;
    }

    pthread_mutex_unlock(&mutex);
    printf("Cl: Error - Clave %d no encontrada.\n", key);
    return -1;
}

// **Verifica si una clave existe**
int exist(int key) {
    pthread_mutex_lock(&mutex);
    int index = hash(key);
    Tupla *actual = tabla[index];

    while (actual) {
        if (actual->key == key) {
            pthread_mutex_unlock(&mutex);
            printf("Cl: La clave %d existe.\n", key);
            return 1;
        }
        actual = actual->next;
    }

    pthread_mutex_unlock(&mutex);
    printf("Cl: La clave %d no existe.\n", key);
    return 0;
}

