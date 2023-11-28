#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include "array.h"
#include "util.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/sem.h>
#include<pthread.h>
#include<stdlib.h>
#include<semaphore.h>
#include<sys/time.h>
#include<pthread.h>
#define MAX_INPUT_FILES 100
#define MAX_REQUESTER_THREADS 10
#define MAX_RESOLVER_THREADS 10
#define MAX_IP_LENGTH INET6_ADDRSTRLEN
#define THREAD_NUM (MAX_REQUESTER_THREADS + MAX_RESOLVER_THREADS)

// Shared Buffer Struct

typedef struct{
    char** inputFile;
    FILE* serviceTxt;
    FILE* resolverTxt;
    sem_t fileLock;
    sem_t serviceLock;
    sem_t resultsLock;
    sem_t poisonPill;
    array* sharedBuffer;
    int index;
    int fileCount;
} multithread;

void* requesters(void * s);
void* resolvers(void * s);

#endif
