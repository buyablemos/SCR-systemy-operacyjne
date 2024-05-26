#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_THREADS 32

pthread_barrier_t barrier;

void *print(void* i) {
     long taskid;
     taskid = (long) i;
    int wynik;
    pthread_barrier_wait(&barrier);
    printf("Hello OpSys. Written by thread ID %ld\n", taskid);
    
    for (int b=0; b<10; ++b) wynik += log(b);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_t taskids[NUM_THREADS];

    int error;
    
    pthread_barrier_init(&barrier, NULL, NUM_THREADS + 1);

    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Zostaje utworzony watek %d\n", i);
        taskids[i]=i;
        error = pthread_create(&threads[i], NULL, print, (void *) taskids[i]);
        if (error) {
            printf("Blad w pthread_create%d\n", error);
            exit(-1);
        }
    }
    pthread_barrier_wait(&barrier);
    // Czeka na watki
    for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
    }
    
   pthread_barrier_destroy(&barrier);
   pthread_exit(NULL);

    return 0;
}
