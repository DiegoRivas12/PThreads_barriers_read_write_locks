#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

int counter = 0;  // Variable compartida para contar los hilos que han alcanzado la barrera
int thread_count; // Número total de hilos
sem_t count_sem;  // Semáforo para proteger la variable counter
sem_t barrier_sem; // Semáforo para bloquear los hilos que no han llegado a la barrera

// Función que simula el trabajo de cada hilo
void* ThreadWork(void* rank) {
    long my_rank = (long) rank;
    
    // Sección crítica: acceso protegido por semáforo count_sem
    sem_wait(&count_sem); // Entrar en la sección crítica
    counter++; // Incrementar el contador
    sem_post(&count_sem); // Salir de la sección crítica

    // Si soy el último hilo en llegar, libero todos los hilos de la barrera
    if (counter == thread_count) {
        counter = 0;  // Resetear el contador para la siguiente barrera
        sem_post(&count_sem); // Liberar el semáforo count_sem para el siguiente hilo
        for (int j = 0; j < thread_count - 1; j++) {
            sem_post(&barrier_sem);  // Despertar a todos los hilos bloqueados en la barrera
        }
    } else {
        sem_wait(&barrier_sem);  // Esperar a que todos los hilos lleguen a la barrera
    }

    printf("Thread %ld passed the barrier\n", my_rank); // Mensaje después de pasar la barrera
    return NULL;
}

int main(int argc, char* argv[]) {
    // Verificar que el número de hilos es válido
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of threads>\n", argv[0]);
        return -1;
    }
    
    thread_count = strtol(argv[1], NULL, 10);
    
    pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));
    
    // Inicializar los semáforos
    sem_init(&count_sem, 0, 1); // Inicializa count_sem con valor 1 (para acceso exclusivo)
    sem_init(&barrier_sem, 0, 0); // Inicializa barrier_sem con valor 0 (bloquear todos los hilos)

    // Crear los hilos
    for (long thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, ThreadWork, (void*) thread);
    }

    // Esperar que todos los hilos terminen
    for (long thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    // Destruir los semáforos
    sem_destroy(&count_sem);
    sem_destroy(&barrier_sem);
    free(thread_handles);
    
    return 0;
}
