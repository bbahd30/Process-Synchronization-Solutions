#include <pthread.h>
#include "../Semaphore.h"
#include <stdlib.h>
#include <stdio.h>

#define Max 8
#define SizeofBuffer 10

struct Semaphore not_full;
struct Semaphore not_empty;
int in =0;
int out = 0;
static int* buffer;

pthread_mutex_t mutex;

void Producer_Process(void *producer_no)
{
    int object;
    for(int i=0;i<Max;i++)
    {
        // sem_wait(&not_full);
        object = rand(); 
        pthread_mutex_lock(&mutex);
        buffer[in] = object;
        printf("Producer %d Inserted object %d at %d \n",*((int *)producer_no), buffer[in],in);
        in = in+1;
        in = in%SizeofBuffer;
        pthread_mutex_unlock(&mutex);
        sem_post(&not_empty); //signals notempty      
    }
}

void Consumer_Process(void *consumer_no)
{
    for(int i=0;i<Max;i++)
    {
        sem_wait(&not_empty);
        pthread_mutex_lock(&mutex);
        int object = buffer[out];
        printf("Consumer %d Removed object %d at %d \n",*((int *)consumer_no), buffer[out],out);
        buffer[out] = -1;
        out = out+1;
        out = out%SizeofBuffer;
        pthread_mutex_unlock(&mutex);
        //sem_post(&not_full); //signals not_full
    }
}

int main()
{
    pthread_t producers[8];
    pthread_t consumers[8];
    pthread_mutex_init(&mutex,NULL);
    // sem_init(&not_full, 0, SizeofBuffer);
    sem_init(&not_empty, 0);
    buffer = (int*)malloc(SizeofBuffer * sizeof(int));

    int nos[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    for(int i = 0; i < 8; i++) {
        pthread_create(&producers[i], NULL, (void *)&Producer_Process, (void *)&nos[i]);
    }
    
    for(int i = 0; i < 8; i++) {
        pthread_create(&consumers[i], NULL, (void *)&Consumer_Process, (void *)&nos[i]);
    }

    for(int i = 0; i < 8; i++) {
        pthread_join(producers[i], NULL);
    }
    for(int i = 0; i < 8; i++) {
        pthread_join(consumers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&not_empty);
    // sem_destroy(&not_full);

    return 0;
}