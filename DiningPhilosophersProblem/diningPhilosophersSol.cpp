#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>
#include "../MeraSemaphore.h"

pthread_t philosopher[5];
Merasemaphore chopstick[5];

int count[5];

void* philosopher_thread_function(void* i){
    int n = *((int*)i);
    printf("Philosopher %d is THINKING \n", n);
    if(n<5){
        sem_wait(&chopstick[n]);
        sem_wait(&chopstick[(n+1)%5]);
        printf("Philosopher %d is EATING \n", n);
        sleep(2);
        sem_post(&chopstick[n]);
        sem_post(&chopstick[(n+1)%5]);
    }else{
        sem_wait(&chopstick[(n+1)%5]);
        sem_wait(&chopstick[n]);
        printf("Philosopher %d is EATING \n", n);
        sleep(2);
        sem_post(&chopstick[(n+1)%5]);
        sem_post(&chopstick[n]);
    }
    printf("Philosopher %d is SLEEPING \n", n);
}

int main(){

    for(int i=0; i<5; i++) 
        count[i]=i+1;

    for(int i=0; i<5; i++)
        sem_init(&chopstick[i],1);

    for(int i=0; i<5; i++)
        if(pthread_create(&philosopher[i], NULL, philosopher_thread_function, &count[i]))
            printf("Philosopher thread %d creation failed! \n", count[i]);
    
    for(int i=0; i<5; i++)
        pthread_join(philosopher[i], NULL);

    for(int i=0; i<5; i++)
        sem_destroy(&chopstick[i]);

    exit(0);
    return 0;
}
