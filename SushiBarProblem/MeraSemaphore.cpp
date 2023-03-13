#include<pthread.h>

#ifndef SEMA_H_INCLUDED
#define SEMA_H_INCLUDED

struct MeraSemaphore{
    unsigned int valueOfSemaphore;
    pthread_cond_t conditionVariable;
    pthread_mutex_t conditionLock;
};

void sem_wait(MeraSemaphore *s){
    pthread_mutex_lock(&(s->conditionLock));
    while((s->valueOfSemaphore) == 0){
        pthread_cond_wait(&(s->conditionVariable), &(s->conditionLock));
    }
    (s->valueOfSemaphore)--;
    pthread_mutex_unlock(&(s->conditionLock));
}

void sem_init(MeraSemaphore *s, int initialValueOfSemaphore){
    pthread_cond_init(&(s->conditionVariable), NULL);
    pthread_mutex_init(&(s->conditionLock), NULL);
    s->valueOfSemaphore = initialValueOfSemaphore;
    return;
}

void sem_post(MeraSemaphore *s){
    pthread_mutex_lock(&(s->conditionLock));
    (s->valueOfSemaphore)++;
    pthread_cond_signal(&(s->conditionVariable));
    pthread_mutex_unlock(&(s->conditionLock));
}

void sem_destroy(MeraSemaphore *s){
    while(!(s->valueOfSemaphore)){
        sem_post(s);
    }
    return;
}

void sem_post(MeraSemaphore *s, int valueByWhichSemaphoreShouldBeIncremented){
    pthread_mutex_lock(&(s->conditionLock));
    (s->valueOfSemaphore) += valueByWhichSemaphoreShouldBeIncremented;
    pthread_cond_signal(&(s->conditionVariable));
    pthread_mutex_unlock(&(s->conditionLock));
}

#endif 

