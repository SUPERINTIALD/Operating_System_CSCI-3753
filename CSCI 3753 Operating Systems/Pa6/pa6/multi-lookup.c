
#include "multi-lookup.h"
// Producer reads from input files, puts on shared buffer using array_put, and writes to services.txt

void* requesters(void* s) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    multithread* tInfo = (multithread*)s;
    int count = 0;
    int indexL = 0;
    while (1) {
        // printf("dddddddddddddddddd>");
        sem_wait(&(tInfo->fileLock));
        indexL = tInfo->index++;
        sem_post(&(tInfo->fileLock));
        // printf("Deadkicj>");
        if (indexL >= tInfo->fileCount) {
            gettimeofday(&end, NULL);
            double runtime = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
            // sem_wait(&(tInfo->printMutex));
            printf("Thread %lx serviced %d files in %lf\n", pthread_self(), count, runtime);
            fflush(stderr);
            // sem_post(&(tInfo->printMutex));
            // printf("HELLO");
            // sem_wait(&(tInfo->serviceLock));
            // fprintf(tInfo->serviceTxt,"Thread %lx serviced %d files in %lf\n", pthread_self(), count, runtime);
            // fflush(tInfo->serviceTxt);
            // sem_post(&(tInfo->serviceLock));
            sem_wait(&(tInfo->incrementRequester));
            tInfo->runningRequester--;
            // printf("Decrement running Requester: %d\n", tInfo->runningRequester );
            sem_post(&(tInfo->incrementRequester)); 
            return NULL;
        }
                
        char* filename = tInfo->inputFile[indexL];
        FILE* fd = fopen(filename, "r");
        if (!fd) {
            fflush(stderr);
            // printf("Skip this file");
            continue; // Skip this file
        }else{
        char hostname[MAX_NAME_LENGTH];
        count++;
        while (fgets(hostname, sizeof(hostname), fd)) {
            if (hostname[0] != '\n') {
                hostname[strlen(hostname) - 1] = '\0';
                array_put(tInfo->sharedBuffer, hostname);
                sem_wait(&(tInfo->serviceLock));
                fprintf(tInfo->serviceTxt, "%s\n", hostname);
                fflush(tInfo->serviceTxt);
                sem_post(&(tInfo->serviceLock));
                }
            }
        }
        fclose(fd);
    }
   
    return NULL;
}
void* resolvers(void* s) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    multithread* tInfo = (multithread*)s;
    char* hostname = malloc(MAX_NAME_LENGTH);
    char * ip = malloc(MAX_IP_LENGTH);
    int count = 0;
    if(hostname == NULL || ip == NULL) {
        exit(-1);
    }
    while (1) {
        // Get hostname from the shared buffer
        if ((tInfo->runningRequester) == 0 && tInfo->sharedBuffer->top == 0) {
                // Poison pill detected, exit the thread, and free corresponding IP, and hostname
                gettimeofday(&end, NULL);
                double runtime = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
                // sem_wait(&tInfo->printMutex);
                printf("Thread %lx resolved %d hostnames in %lf seconds\n", pthread_self(), count,runtime);
                // sem_post(&tInfo->printMutex);
                free(hostname);
                free(ip);
                return NULL;
        }
        array_get(tInfo->sharedBuffer, &hostname);    
        // Resolve hostname to IP address
        if(dnslookup(hostname, ip, MAX_IP_LENGTH) == 0){
            sem_wait(&(tInfo->resultsLock));
            fprintf(tInfo->resolverTxt, "%s, %s\n", hostname, ip);
            // fflush(tInfo->resolverTxt);
            sem_post(&(tInfo->resultsLock));
            count++;

        }else{
            // sem_wait(&(tInfo->resultsLock));
            strcpy(ip, "NOT_RESOLVED");
            // sem_post(&(tInfo->resultsLock));
            sem_wait(&(tInfo->resultsLock));
            fprintf(tInfo->resolverTxt, "%s", hostname);
            // fflush(tInfo->resolverTxt);
            sem_post(&(tInfo->resultsLock));
        }
    }
    return NULL;
}



int main(int argc, char* argv[]) {
    // printf("Program start: \n");
    struct timeval start, end;
    // printf("Requester start: ");
    gettimeofday(&start, NULL);    
    if (argc < 6) {
        printf("Not enough argc");
        return -1;
    }
    int numRequesters = atoi(argv[1]);
    int numResolvers = atoi(argv[2]);

    if (numRequesters > MAX_REQUESTER_THREADS || numResolvers > MAX_RESOLVER_THREADS || numRequesters < 1 || numResolvers < 1) {
        fprintf(stderr, "Error: Invalid num of threads\n");
        exit(EXIT_FAILURE);
    }

    multithread mt;
    mt.serviceTxt = fopen(argv[3], "w");
    mt.resolverTxt = fopen(argv[4], "w");
    mt.inputFile = &argv[5];

    mt.index = 0;
    mt.fileCount = (argc - 5);//counts only input file name

    sem_init(&mt.fileLock, 0, 1);
    sem_init(&mt.serviceLock, 0, 1);
    sem_init(&mt.resultsLock, 0, 1);
    // sem_init(&mt.printMutex,0,1);
    sem_init(&mt.incrementRequester, 0,1);
    array sharedBuffer;
    array_init(&sharedBuffer);
    mt.sharedBuffer = &sharedBuffer;

    pthread_t requesterThreads[numRequesters];
    pthread_t resolverThreads[numResolvers];
    mt.runningRequester = 0;

    for (int i = 0; i < numRequesters; i++) {
        pthread_create(&requesterThreads[i], NULL, requesters, &mt);
        sem_wait(&(mt.incrementRequester));
        mt.runningRequester++;
        // printf("This is num Requester %d\n",mt.runningRequester );
        sem_post(&(mt.incrementRequester));

    }

    for (int i = 0; i < numResolvers; i++) {
        pthread_create(&resolverThreads[i], NULL, resolvers, &mt);
        
    }
    for (int i = 0; i < numRequesters; i++) {
        pthread_join(requesterThreads[i], NULL);
    }
    // for(int i = 0; i < numResolvers; i++){
    //     sem_post(&mt.signalEnd);
    // }
    for (int i = 0; i < numResolvers; i++) {
        pthread_join(resolverThreads[i], NULL);
    }
    fclose(mt.serviceTxt);
    fclose(mt.resolverTxt);
    sem_destroy(&mt.fileLock);
    sem_destroy(&mt.serviceLock);
    sem_destroy(&mt.resultsLock);
    // sem_destroy(&mt.printMutex);
    sem_destroy(&mt.incrementRequester);
    array_free(&sharedBuffer);

    gettimeofday(&end, NULL);
    double runtime = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Total time is %lf seconds\n", runtime);
    return 0;
}   
