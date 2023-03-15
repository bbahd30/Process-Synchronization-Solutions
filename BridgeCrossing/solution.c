#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include "../Semaphore.h"

// semaphore emutex -> provides priority access to the car arrived first
//      releases when allowed to cross bridge (the critical section)
//      else waits by looping without releasing the semaphore emutex

// semaphore mutex -> allows to update the shared variable carsOnBridge, when satisfies the conditon
//      to prevent the race conditon among the cars on the same side allowed to cross, for updating the value of number of cars on bridge
struct Semaphore emutex, leftSem, rightSem;
int numCars;
int rightCars, leftCars;
int carsOnBridge;
int currentDirection;
int carsToCross[2] = {0};

int min(int num1, int num2)
{
    return (num1 > num2) ? num2 : num1;
}

// functions for stimulating car crossing, arriving and leaving the bridge.
void crossingBridge(int carId, int direction)
{
    if (direction == 1)
    {
        printf("Car %d (R) ---> ---> crossing.\n", carId);
    }
    else
    {
        printf("Car %d \t\t\t\t(L) <--- <--- crossing.\n", carId);
    }
}

void bridgeArrived(int carId, int direction)
{
    if (direction == 1)
    {
        printf("Car %d (R) ---> ---> arrived.\n", 1);
    }
    else
    {
        printf("Car %d \t\t\t\t(L) <--- <--- arrived.\n", carId);
    }
}

void leavingBridge(int carId, int direction)
{
    if (direction == 1)
    {
        printf("Car %d (R) ---> ---> leaving.\n", carId);
    }
    else
    {
        printf("Car %d \t\t\t\t(L) <--- <--- leaving.\n", carId);
    }
}

void *rightCar(void *arg)
{
    int carId = *(int *)arg;
    // as the right car arrives its shows its arrival
    // after arriving tries to aquire the emutex semaphore to enter the bridge
    // thus tracking the car arrived first by being aquired by that thread

    // is aquired by the car if there are no other cars waiting for their turn to cross the bridge
    // as if a car is not allowed to cross, it waits without releasing the mutex semaphore
    // thus preventing the cars arriving after the waiting car
    // hence, prevents starvation of the car on opposite side

    sem_wait(&emutex);
    bridgeArrived(carId, 1);
    carsToCross[1]++;

    if ((carsToCross[0] == 0) && (carsOnBridge < 3 && (currentDirection == 1 || currentDirection == 0)))
    {
        carsOnBridge++;
        currentDirection = 1;
        carsToCross[1]--;
        sem_post(&rightSem); // allowing to access bridge
    }
    sem_post(&emutex);

    sem_wait(&rightSem); // will aquire only if access to bridge allowed by satisfying the conditon

    crossingBridge(carId, 1);
    sleep(2);

    // sem_post(&emutex);
    sem_wait(&emutex);
    leavingBridge(carId, 1);
    carsOnBridge--;

    if ((carsOnBridge == 0) && carsToCross[0] > 0)
    {
        int carWaiting = min(3, carsToCross[0]);
        currentDirection = 0;
        while (carWaiting > 0)
        {
            sem_post(&leftSem);
            carsOnBridge++;
            carsToCross[0]--;
            carWaiting--;
        }
    }
    else if (carsToCross[0] == 0 && carsToCross[1] > 0)
    {
        sem_post(&rightSem);
        carsOnBridge++;
        carsToCross[1]--;
    }
    sem_post(&emutex);
    pthread_exit(NULL);
}
void *leftCar(void *arg)
{

    int carId = *(int *)arg;
    // as the right car arrives its shows its arrival
    // after arriving tries to aquire the emutex semaphore to enter the bridge
    // thus tracking the car arrived first by being aquired by that thread

    // is aquired by the car if there are no other cars waiting for their turn to cross the bridge
    // as if a car is not allowed to cross, it waits without releasing the mutex semaphore
    // thus preventing the cars arriving after the waiting car
    // hence, prevents starvation of the car on opposite side

    sem_wait(&emutex);
    bridgeArrived(carId, -1);
    carsToCross[0]++;
    if ((carsToCross[1] == 0) && (carsOnBridge < 3 && (currentDirection == -1 || currentDirection == 0)))
    {
        carsOnBridge++;
        currentDirection = -1;
        carsToCross[0]--;
        sem_post(&leftSem); // allowing to access bridge
    }
    sem_post(&emutex);

    sem_wait(&leftSem); // will aquire only if access to bridge allowed by satisfying the conditon

    crossingBridge(carId, -1);
    sleep(2);

    // sem_post(&emutex);
    sem_wait(&emutex);
    leavingBridge(carId, -1);
    carsOnBridge--;

    if ((carsOnBridge == 0) && carsToCross[1] > 0)
    {
        int carWaiting = min(3, carsToCross[1]);
        currentDirection = 0;
        while (carWaiting > 0)
        {
            sem_post(&rightSem);
            carsOnBridge++;
            carsToCross[1]--;
            carWaiting--;
        }
    }
    else if (carsToCross[1] == 0 && carsToCross[0] > 0)
    {
        sem_post(&leftSem);
        carsOnBridge++;
        carsToCross[0]--;
    }
    sem_post(&emutex);
    pthread_exit(NULL);
}

int main()
{
    printf("Enter the number of cars: ");
    scanf("%d", &numCars);
    printf("Enter the number of cars heading towards left: ");
    scanf("%d", &leftCars);

    rightCars = numCars - leftCars;

    currentDirection = 0;
    carsOnBridge = 0;

    sem_init(&emutex, 1);
    sem_init(&leftSem, 0);
    sem_init(&rightSem, 0);

    pthread_t cars[numCars];
    int threadArgs[numCars];

    int i = 0;
    while (i < numCars)
    {
        threadArgs[i] = i + 1;
        int randomDirection = rand() % 2;
        if (randomDirection == 0 && rightCars > 0)
        {
            pthread_create(&cars[i], NULL, rightCar, (void *)&threadArgs[i]);
            rightCars--;
            i++;
        }
        else if (randomDirection == 1 && leftCars > 0)
        {
            pthread_create(&cars[i], NULL, leftCar, (void *)&threadArgs[i]);
            leftCars--;
            i++;
        }
    }

    for (int i = 0; i < numCars; i++)
    {
        pthread_join(cars[i], NULL);
    }

    sem_destroy(&emutex);
    sem_destroy(&leftSem);
    sem_destroy(&rightSem);
    return 0;
}
