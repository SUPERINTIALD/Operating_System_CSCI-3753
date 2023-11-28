#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h> 
#include <unistd.h> 

#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_SIZE 8
#define MAX_NAME_LENGTH 63                 // max elements in stack

typedef struct {
    char * array[ARRAY_SIZE];                  // storage array for integers
    int top;                                // array index indicating where the top is
    sem_t sp_avail; // put
    sem_t item_avail; // get
    sem_t mutex; // binary semaphore
} array;

int  array_init(array *s);                   // Init the array
int  array_put (array *s, char *hostname);   // place item in array, signal when full
int  array_get (array *s, char **hostname);  // remove item in  array, signal when empty
void array_free(array *s);                   // free array 

#endif
