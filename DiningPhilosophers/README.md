# Dining Philosophers Synchronization Problem

## Problem Statement

The dining philosopher's problem is the classical problem of synchronization which says that a number of philosophers are sitting around a circular table and their job is to think and eat alternatively. A bowl of noodles is placed at the center of the table along with same number of chopsticks as there are philosophers at the table. To eat, a philosopher needs both their right and a left chopstick. A philosopher can only eat if both immediate left and right chopsticks of the philosopher is available. In case if both immediate left and right chopsticks of the philosopher are not available then the philosopher puts down their (either left or right) chopstick and starts thinking again.

## Solution

POSIX threads will be used to create and manage threads in the solution. Each philosopher is represented using a thread with each philosopher carrying out his execution parallely. As each chopstick is a shared resource between philosophers sitting be on the left and right of the chopstick, a semaphore to ensure mutual exclusion is required per chopstick. 

```
/* Count of philosophers present at dining table */
#define COUNT 7

/* Assigning individual threads for every philosopher process */ 
pthread_t philosopher[COUNT];

/* Each chopstick is a shared resource, so assigning one semaphore per chopstick to ensure mutual exclusion */
Semaphore chopstick[COUNT];

int philosopherNumber[COUNT];

/* Philosopher function to run whenever a philosopher thread executes */
void* philosopher_thread_function(void* philosopherNumber){
    // Code 
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

```

### 1.1 Initial Solution (Deadlock Prone)

```
void* philosopher_thread_function(void* philosopherNumber){

    /* The number of philosopher thread executing */
    int n = *((int*)philosopherNumber);

    /* Philosopher performing thinking action */
    printf("Philosopher %d is THINKING \n", n);

    /* Waiting for right and left chopsticks */
    sem_wait(&chopstick[n]);
    sem_wait(&chopstick[(n+1)%5]);

    /* Philosopher performing eating action */
    printf("Philosopher %d is EATING \n", n);
    sleep(2);

    /* Signaling availability of right and left chopsticks */
    sem_post(&chopstick[n]);
    sem_post(&chopstick[(n+1)%5]);

    /* Philosopher going to sleep */
    printf("Philosopher %d is SLEEPING \n", n);
}
```

Assuming clockwise numbering of philosophers around the table, each philosopher first waits for the chopstick on his right and then for the left chopstick before entering into EATING state. Whenever both the chopsticks become available, philosopher starts eating (takes 2 seconds). Once philosopher is done eating, it releases both the held chopsticks making them available to adjacent philosophers. 

But this solution is deadlock prone, as all the philosophers are checking for the right chopsticks first there might occur a situation where all philosophers simultaneously pick there right chopsticks, now every philosopher will keep waiting for the left chopstick in a circular wait leading to Deadlock condition.

### 1.2 Deadlock Free Solution

```
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
```

Unlike solution 1.1, here each philosopher will wait for his right or left chopstick first based on his position on the table. All even numbered philosophers will first wait for right chopstick and then look for the left one whereas odd numbered philosophers will do the exact opposite preventing circular wait, and hence in turn Deadlock condition.