#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

void* producer_thread(void* arg) {
    array* shared_array = (array*)arg;

    for (int i = 0; i < 5; i++) {
        char hostname[MAX_NAME_LENGTH];
        snprintf(hostname, sizeof(hostname), "host_%d", i);

        array_put(shared_array, hostname);
        printf("Producer: Produced %s\n", hostname);

        sleep(1); // Sleep for 2 seconds
    }

    return NULL;
}

void* consumer_thread(void* arg) {
    array* shared_array = (array*)arg;

    for (int i = 0; i < 5; i++) {
        char* hostname = NULL;

        array_get(shared_array, &hostname);
        printf("Consumer: Consumed %s\n", hostname);
        free(hostname);

        sleep(1); // Sleep for 2 seconds
    }

    return NULL;
}

int main() {
    printf("Main: Starting the program\n");
    array shared_array;
    // printf("Check 1");
    array_init(&shared_array);
    // printf("Check init");

    pthread_t producer_threads[NUM_PRODUCERS];
    pthread_t consumer_threads[NUM_CONSUMERS];
    // printf("Check 2");

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producer_threads[i], NULL, producer_thread, &shared_array);
    }
    // printf("Check 3");

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumer_threads[i], NULL, consumer_thread, &shared_array);
    }
    // printf("Check 4");

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producer_threads[i], NULL);
    }
    // printf("Check 5");

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumer_threads[i], NULL);
    }

    printf("Main: All threads have finished\n");

    array_free(&shared_array);
    printf("Main: Exiting the program\n");
    return 0;
}
