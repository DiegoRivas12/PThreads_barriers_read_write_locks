#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>  // Para intptr_t

// Variables compartidas
int counter = 0;
int thread_count;
pthread_mutex_t barrier_mutex;

void* ThreadWork(void* rank) {
    intptr_t my_rank = (intptr_t) rank;

    printf("Hilo %ld llegó a la barrera\n", my_rank);

    // Barrera
    pthread_mutex_lock(&barrier_mutex);
    counter++;
    pthread_mutex_unlock(&barrier_mutex);

    while (counter < thread_count);  // Busy-waiting

    printf("Hilo %ld pasando la barrera\n", my_rank);

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <número de hilos>\n", argv[0]);
        exit(1);
    }

    thread_count = strtol(argv[1], NULL, 10);
    pthread_t thread_handles[thread_count];

    pthread_mutex_init(&barrier_mutex, NULL);

    for (intptr_t thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, ThreadWork, (void*) thread);
    }

    for (int thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    pthread_mutex_destroy(&barrier_mutex);

    return 0;
}
