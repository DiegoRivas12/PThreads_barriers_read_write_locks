#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// Variables compartidas
int counter = 0;                      // Cuenta los hilos que llegaron a la barrera
int thread_count;                     // Número de hilos
pthread_mutex_t mutex;                // Mutex para proteger la variable `counter`
pthread_cond_t cond_var;              // Variable de condición para sincronización

void* thread_work(void* rank) {
    long my_rank = (long) rank;       // Obtener el índice del hilo

    // Sección crítica donde cada hilo incrementa el contador al llegar a la barrera
    pthread_mutex_lock(&mutex);        // Bloquea el mutex

    counter++;                         // Incrementa el contador de hilos que llegaron
    printf("Hilo %ld llegó a la barrera, contador: %d\n", my_rank, counter);

    if (counter == thread_count) {     // Si todos los hilos llegaron
        counter = 0;                   // Resetea el contador
        pthread_cond_broadcast(&cond_var);  // Desbloquea todos los hilos bloqueados
    } else {
        // Si no todos los hilos llegaron, espera en la condición
        while (pthread_cond_wait(&cond_var, &mutex) != 0);  // Desbloquea y espera
    }

    pthread_mutex_unlock(&mutex);      // Desbloquea el mutex

    printf("Hilo %ld pasó la barrera\n", my_rank);

    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t* thread_handles;         // Array de manejadores de hilos

    // Validación de los argumentos
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <número de hilos>\n", argv[0]);
        return -1;
    }

    // Inicialización de variables
    thread_count = strtol(argv[1], NULL, 10);  // Obtiene el número de hilos desde la línea de comandos
    thread_handles = malloc(thread_count * sizeof(pthread_t));  // Reserva memoria para los hilos

    // Inicializa el mutex y la variable de condición
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_var, NULL);

    // Crear los hilos
    for (long i = 0; i < thread_count; i++) {
        pthread_create(&thread_handles[i], NULL, thread_work, (void*) i);
    }

    // Esperar a que todos los hilos terminen
    for (long i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
    }

    // Liberar los recursos
    free(thread_handles);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);

    return 0;
}
