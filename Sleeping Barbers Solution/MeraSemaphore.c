#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

struct MeraSemaphore{

    unsigned int ValueOfSemaphore;
    pthread_cond_t ConditionVariable;
    pthread_mutex_t ConditionLock;

}/* Declare the Semaphore vairables here*/;

void sem_wait(struct MeraSemaphore *s){
    pthread_mutex_lock(&(s->ConditionLock));
    while((s->ValueOfSemaphore) == 0){
        pthread_cond_wait(&(s->ConditionVariable), &(s->ConditionLock));
    }
    (s->ValueOfSemaphore)--;
    pthread_mutex_unlock(&(s->ConditionLock));
}

void sem_init(struct MeraSemaphore *s, int InitialValueOfSemaphore){
    pthread_cond_init(&(s->ConditionVariable), NULL);
    pthread_mutex_init(&(s->ConditionLock), NULL);
    s->ValueOfSemaphore = InitialValueOfSemaphore;
    return;
}

void sem_post(struct MeraSemaphore *s){
    pthread_mutex_lock(&(s->ConditionLock));
    (s->ValueOfSemaphore)++;
    pthread_cond_signal(&(s->ConditionVariable));
    pthread_mutex_unlock(&(s->ConditionLock));
}

void sem_destroy(struct MeraSemaphore *s){
    while(!(s->ValueOfSemaphore)){
        sem_post(s);
    }
    return;
}
