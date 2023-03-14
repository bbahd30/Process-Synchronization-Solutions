#include<pthread.h>

#ifndef SEMA_H_INCLUDED
#define SEMA_H_INCLUDED
// This is a preprocessing approach for avoiding the
// inclusion of a header file numerous times, which
// can be troublesome for a variety of reasons.

//defining a struct
struct Semaphore{
    unsigned int valueOfSemaphore;
    pthread_cond_t conditionVariable;
    pthread_mutex_t conditionLock;
};

void sem_wait(Semaphore *s){
    pthread_mutex_lock(&(s->conditionLock));
    while((s->valueOfSemaphore) == 0){
        pthread_cond_wait(&(s->conditionVariable), &(s->conditionLock));
        //pthread_cond_wait does tḥe following:
        //1. unlocks the mutex
		//2. block the calling thread until another thread signals the condition variable cond
		//3. relocks mutex
    }	
    (s->valueOfSemaphore)--;
    pthread_mutex_unlock(&(s->conditionLock));
}

void sem_init(Semaphore *s, int initialValueOfSemaphore){
    pthread_cond_init(&(s->conditionVariable), NULL);
    pthread_mutex_init(&(s->conditionLock), NULL);
    s->valueOfSemaphore = initialValueOfSemaphore;
    //intializing a semaphore
    return;
}

void sem_post(Semaphore *s){
    pthread_mutex_lock(&(s->conditionLock));
    (s->valueOfSemaphore)++;
    pthread_cond_signal(&(s->conditionVariable));
    // Unblock at least one thread that is blocked on the
    // specified condition variable, cond. If more than one
    // thread is blocked, the order in which the threads
    // are unblocked is unspecified.
    pthread_mutex_unlock(&(s->conditionLock));
}

void sem_destroy(Semaphore *s){
    while(!(s->valueOfSemaphore)){
        sem_post(s);
    }
    // sem_destroy() destroys the unnamed semaphore at the address
    // pointed to by sem.
}

void sem_post(Semaphore *s, int valueByWhichSemaphoreShouldBeIncremented) {
    pthread_mutex_lock(&(s->conditionLock));
    (s->valueOfSemaphore) += valueByWhichSemaphoreShouldBeIncremented;
    pthread_cond_signal(&(s->conditionVariable));
    pthread_mutex_unlock(&(s->conditionLock));
}

#endif 

