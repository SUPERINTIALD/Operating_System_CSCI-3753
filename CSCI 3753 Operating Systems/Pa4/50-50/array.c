#include "array.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h> 
#include <unistd.h> 

int  array_init(array *s){ 
    // printf("Start init\n");
    memset(s->array, 0 , sizeof(char) * ARRAY_SIZE); //set array, and init all values to 0 in ARRAY_SIZE
    // printf("memset start\n");
    s-> top = 0;
    // printf("sp_avail start\n");
    if(sem_init(&(s->sp_avail), 1, ARRAY_SIZE) != 0){//init space avail
        printf("Error, sp_avail failed to initialize");
        sem_destroy(&(s->sp_avail));
        return -1;
    }

    if(sem_init(&(s->item_avail), 1, 0) != 0){//init item
        printf("Error, item_avail failed to initialize");
        sem_destroy(&(s->sp_avail));
        sem_destroy(&(s->item_avail));
    }
    if(sem_init(&(s->mutex), 0,1) != 0){ //init mutex
        printf("Error, mutex failed to initialize");
        sem_destroy(&(s->sp_avail));
        sem_destroy(&(s->item_avail));
    }
    return 0;
}                  
int  array_put (array *s, char *hostname){ // place element into the array, block when full
    
    sem_wait(&(s->sp_avail)); // Wait until non-empty/items in after array_put
    sem_wait(&(s->mutex)); // Acquire the mutex
    
    if (s->top >= ARRAY_SIZE){
        return -1;    // return failed if the stack is full
    }
    s->array[s->top++] = hostname;
    sem_post(&(s->mutex)); // Release mutex
    sem_post(&(s->item_avail)); // Signal buffer isn't full
    
    
    return 0;
}  
int  array_get (array *s, char **hostname){
    sem_wait(&(s->item_avail)); // Wait for an empty slot/array_get is done
    sem_wait(&(s->mutex)); // Acquire the mutex
    
    if (s->top < 1){
        return -1;
    }              // return failed if the stack is empty
    *hostname = s->array[--s->top];

    
    sem_post(&(s->mutex)); // Release the mutex
    sem_post(&(s->sp_avail)); // Signal that the buffer is not empty
    return 0;
}  
void array_free(array *s){  // Free everything
    sem_destroy(&(s->sp_avail));
    sem_destroy(&(s->item_avail));
    sem_destroy(&(s->mutex));
}            