#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>
#include "../MeraSemaphore.h"

pthread_t philosopher[5];
// pthread_mutex_t chopstick[5];
// atomic_bool chopstick[5];
Merasemaphore chopstick[5];

int count[5];

void* philosopher_thread_function(void* i){
    int n = *((int*)i);
    printf("Philosopher %d is THINKING \n", n);
    // pthread_mutex_lock(&chopstick[n]);
    // pthread_mutex_lock(&chopstick[(n+1)%5]);
    // while(!chopstick[n]) ;
    // while(!chopstick[(n+1)%5]) ;
    if(n<5){
        sem_wait(&chopstick[n]);
        sem_wait(&chopstick[(n+1)%5]);
        printf("Philosopher %d is EATING \n", n);
        sleep(2);
        // pthread_mutex_unlock(&chopstick[n]);
        // pthread_mutex_unlock(&chopstick[(n+1)%5]);
        // chopstick[n]=true;
        // chopstick[(n+1)%5]=true;
        sem_post(&chopstick[n]);
        sem_post(&chopstick[(n+1)%5]);
        printf("Philosopher %d is SLEEPING \n", n);
    }else{
        sem_wait(&chopstick[(n+1)%5]);
        sem_wait(&chopstick[n]);
        printf("Philosopher %d is EATING \n", n);
        sleep(2);
        // pthread_mutex_unlock(&chopstick[n]);
        // pthread_mutex_unlock(&chopstick[(n+1)%5]);
        // chopstick[n]=true;
        // chopstick[(n+1)%5]=true;
        sem_post(&chopstick[(n+1)%5]);
        sem_post(&chopstick[n]);
        printf("Philosopher %d is SLEEPING \n", n);
    }
    // sem_wait(&chopstick[n]);
    // sem_wait(&chopstick[(n+1)%5]);
    // printf("Philosopher %d is EATING \n", n);
    // sleep(2);
    // // pthread_mutex_unlock(&chopstick[n]);
    // // pthread_mutex_unlock(&chopstick[(n+1)%5]);
    // chopstick[n]=true;
    // chopstick[(n+1)%5]=true;
    // printf("Philosopher %d is SLEEPING \n", n);
}

int main(){

    for(int i=0; i<5; i++) 
        count[i]=i+1;

    // for(int i=0; i<5; i++)
    //     if(pthread_mutex_init(&chopstick[i], NULL)==-1)
    //         printf("Chopstick mutex %d initialisation failed! \n", count[i]);

    for(int i=0; i<5; i++)
        sem_init(&chopstick[i],1);

    // for(int i=0; i<5; i++)
    //     chopstick[i]=true;

    for(int i=0; i<5; i++)
        if(pthread_create(&philosopher[i], NULL, philosopher_thread_function, &count[i]))
            printf("Philosopher thread %d creation failed! \n", count[i]);
    
    for(int i=0; i<5; i++)
        pthread_join(philosopher[i], NULL);

    // for(int i=0; i<5; i++)
    //     if(pthread_mutex_destroy(&chopstick[i]))
    //         printf("Chopstick mutex %d destruction failed! \n", count[i]);

    for(int i=0; i<5; i++)
        sem_destroy(&chopstick[i]);

    exit(0);
    return 0;
}
