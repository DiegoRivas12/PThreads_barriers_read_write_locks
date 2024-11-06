#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct list_node_s {
    int data;
    struct list_node_s* next;
    pthread_mutex_t mutex;  // Mutex por nodo
};

struct list_node_s* head = NULL;

// Función Member: verificar si un valor está en la lista
int Member(int value) {
    struct list_node_s* temp_p = head;

    while (temp_p != NULL && temp_p->data < value) {
        pthread_mutex_lock(&temp_p->mutex);  // Bloquear el nodo actual
        if (temp_p->next != NULL) {
            pthread_mutex_lock(&temp_p->next->mutex);  // Bloquear el siguiente nodo
        }
        pthread_mutex_unlock(&temp_p->mutex);  // Desbloquear el nodo actual
        temp_p = temp_p->next;
    }

    if (temp_p != NULL && temp_p->data == value) {
        pthread_mutex_unlock(&temp_p->mutex);  // Desbloquear el nodo encontrado
        return 1;  // Valor encontrado
    }
    return 0;  // Valor no encontrado
}

// Función Insert: insertar un valor en la lista
int Insert(int value) {
    struct list_node_s* curr_p = head;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        if (temp_p == NULL) {
            perror("malloc error");
            return 0;
        }
        temp_p->data = value;
        temp_p->next = curr_p;
        pthread_mutex_init(&temp_p->mutex, NULL);  // Inicializar el mutex del nodo

        if (pred_p == NULL) {  // Insertar al principio
            head = temp_p;
        } else {
            pred_p->next = temp_p;
        }

        return 1;  // Inserción exitosa
    }
    return 0;  // Valor ya en la lista
}

// Función Delete: eliminar un valor de la lista
int Delete(int value) {
    struct list_node_s* curr_p = head;
    struct list_node_s* pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) {  // Eliminar el primer nodo
            head = curr_p->next;
        } else {
            pred_p->next = curr_p->next;
        }

        pthread_mutex_destroy(&curr_p->mutex);  // Destruir el mutex del nodo
        free(curr_p);

        return 1;  // Eliminación exitosa
    }
    return 0;  // Valor no encontrado
}

int main() {
    // Prueba de inserción y eliminación
    Insert(10);
    Insert(20);
    Insert(30);
    Insert(40);

    printf("Eliminar 20: %s\n", Delete(20) ? "Exitoso" : "Fallido");
    printf("Buscar 20: %s\n", Member(20) ? "Encontrado" : "No encontrado");

    return 0;
}
