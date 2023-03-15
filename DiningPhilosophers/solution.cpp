#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>
#include "../Semaphore.h"

/* Count of philosophers present at dining table */
#define COUNT 7

/* Assigning individual threads for every philosopher process */ 
pthread_t philosopher[COUNT];

/* Each chopstick is a shared resource, so assigning one semaphore per chopstick to ensure mutual exclusion */
Semaphore chopstick[COUNT];

int philosopherNumber[COUNT];

/* Philosopher function to run whenever a pholosopher thread executes */
void* philosopher_thread_function(void* philosopherNumber){

    /* The number of philosopher thread executing */
    int n = *((int*)philosopherNumber);

    /* Philosopher performing thinking action */
    printf("Philosopher %d is THINKING \n", n);

    /* 
    Assuming philosophers are numbered clockwise,
    Even numbered philosopher will first look for right chopstick and then for the left chopstick whereas,
    Odd numbered philosopher will first look for left chopstick and then for the right chopstick
    */
    if(!n%2) {
        /* Waiting for right and left chopsticks */
        sem_wait(&chopstick[n]);
        sem_wait(&chopstick[(n+1)%5]);

        /* Philosopher performing eating action */
        printf("Philosopher %d is EATING \n", n);
        sleep(2);

        /* Signaling availability of right and left chopsticks */
        sem_post(&chopstick[n]);
        sem_post(&chopstick[(n+1)%5]);

    }else {
        /* Waiting for right and left chopsticks */
        sem_wait(&chopstick[(n+1)%5]);
        sem_wait(&chopstick[n]);

        /* Philosopher performing eating action */
        printf("Philosopher %d is EATING \n", n);
        sleep(2);

         /* Signaling availability of right and left chopsticks */
        sem_post(&chopstick[(n+1)%5]);
        sem_post(&chopstick[n]);
    }

    /* Philosopher going to sleep */
    printf("Philosopher %d is SLEEPING \n", n);
}

int main(){

    for(int i=0; i<COUNT; i++) 
        philosopherNumber[i]=i+1;

    // Initializing chopstick semaphores
    for(int i=0; i<COUNT; i++)
        sem_init(&chopstick[i],1);

    // Creating philosopher threads
    for(int i=0; i<COUNT; i++)
        if(pthread_create(&philosopher[i], NULL, philosopher_thread_function, &philosopherNumber[i]))
            printf("Philosopher thread %d creation failed! \n", philosopherNumber[i]);
    
    // Joining philosopher threads
    for(int i=0; i<COUNT; i++)
        if(pthread_join(philosopher[i], NULL))
            printf("Philosopher thread %d joining failed! \n", philosopherNumber[i]);

    // Destroying chopstick semaphores
    for(int i=0; i<COUNT; i++)
        sem_destroy(&chopstick[i]);

    exit(0);
    return 0;
}
