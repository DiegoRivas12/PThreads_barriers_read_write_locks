#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Definición de la estructura de nodo
struct list_node_s {
    int data;
    struct list_node_s* next;
};

// Puntero a la cabeza de la lista y el bloqueo de lectura/escritura
struct list_node_s* head = NULL;
pthread_rwlock_t rwlock;  // bloqueo de lectura/escritura

int Member(int value) {
    struct list_node_s* curr_p;

    // Adquiere el bloqueo de lectura
    pthread_rwlock_rdlock(&rwlock);

    curr_p = head;
    while (curr_p != NULL && curr_p->data < value) {
        curr_p = curr_p->next;
    }

    // Verifica si se encontró el valor
    int found = (curr_p != NULL && curr_p->data == value);

    // Libera el bloqueo de lectura
    pthread_rwlock_unlock(&rwlock);

    return found;
}
int Insert(int value) {
    struct list_node_s* curr_p = head;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;

    // Adquiere el bloqueo de escritura
    pthread_rwlock_wrlock(&rwlock);

    // Recorre la lista para encontrar la posición correcta
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // Si el valor ya está en la lista, no lo insertamos
    if (curr_p != NULL && curr_p->data == value) {
        pthread_rwlock_unlock(&rwlock);
        return 0;
    }

    // Inserta el nuevo nodo
    temp_p = malloc(sizeof(struct list_node_s));
    temp_p->data = value;
    temp_p->next = curr_p;

    if (pred_p == NULL) {
        head = temp_p;
    } else {
        pred_p->next = temp_p;
    }

    // Libera el bloqueo de escritura
    pthread_rwlock_unlock(&rwlock);

    return 1;
}

int Delete(int value) {
    struct list_node_s* curr_p = head;
    struct list_node_s* pred_p = NULL;

    // Adquiere el bloqueo de escritura para realizar la eliminación
    pthread_rwlock_wrlock(&rwlock);

    // Recorre la lista para encontrar el nodo a eliminar
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // Verifica si se encontró el valor
    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) { // Eliminando el primer nodo de la lista
            head = curr_p->next;
        } else { // Eliminando un nodo en el medio o al final
            pred_p->next = curr_p->next;
        }
        free(curr_p);

        // Libera el bloqueo de escritura después de la eliminación
        pthread_rwlock_unlock(&rwlock);
        return 1; // Eliminación exitosa
    } else {
        // Valor no encontrado, libera el bloqueo de escritura
        pthread_rwlock_unlock(&rwlock);
        return 0; // Valor no está en la lista
    }
}

int main() {
    // Inicializa el bloqueo de lectura/escritura
    pthread_rwlock_init(&rwlock, NULL);

    // Pruebas de inserción y búsqueda
    Insert(5);
    Insert(10);
    Insert(3);

    printf("Is 10 in the list? %d\n", Member(10));
    printf("Is 4 in the list? %d\n", Member(4));

    // Destruye el bloqueo de lectura/escritura
    pthread_rwlock_destroy(&rwlock);

    return 0;
}
