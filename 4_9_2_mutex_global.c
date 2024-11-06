#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct list_node_s {
    int data;
    struct list_node_s* next;
};

struct list_node_s* head = NULL;
pthread_mutex_t list_mutex;  // Mutex global para la lista

// Función Member: verificar si un valor está en la lista
int Member(int value) {
    pthread_mutex_lock(&list_mutex);  // Bloquear la lista completa

    struct list_node_s* curr_p = head;
    while (curr_p != NULL && curr_p->data < value) {
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value) {
        pthread_mutex_unlock(&list_mutex);  // Desbloquear
        return 1;  // Valor encontrado
    } else {
        pthread_mutex_unlock(&list_mutex);  // Desbloquear
        return 0;  // Valor no encontrado
    }
}

// Función Insert: insertar un valor en la lista
int Insert(int value) {
    pthread_mutex_lock(&list_mutex);  // Bloquear la lista completa

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
            pthread_mutex_unlock(&list_mutex);  // Desbloquear
            return 0;
        }
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL) {  // Insertar al principio
            head = temp_p;
        } else {
            pred_p->next = temp_p;
        }

        pthread_mutex_unlock(&list_mutex);  // Desbloquear
        return 1;  // Inserción exitosa
    } else {
        pthread_mutex_unlock(&list_mutex);  // Desbloquear
        return 0;  // Valor ya en la lista
    }
}

// Función Delete: eliminar un valor de la lista
int Delete(int value) {
    pthread_mutex_lock(&list_mutex);  // Bloquear la lista completa

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
        free(curr_p);

        pthread_mutex_unlock(&list_mutex);  // Desbloquear
        return 1;  // Eliminación exitosa
    } else {
        pthread_mutex_unlock(&list_mutex);  // Desbloquear
        return 0;  // Valor no encontrado
    }
}

int main() {
    pthread_mutex_init(&list_mutex, NULL);  // Inicializar el mutex global

    // Prueba de inserción y eliminación
    Insert(10);
    Insert(20);
    Insert(30);
    Insert(40);

    printf("Eliminar 20: %s\n", Delete(20) ? "Exitoso" : "Fallido");
    printf("Buscar 20: %s\n", Member(20) ? "Encontrado" : "No encontrado");

    pthread_mutex_destroy(&list_mutex);  // Destruir el mutex global
    return 0;
}
