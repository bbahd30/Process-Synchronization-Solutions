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
struct Semaphore emutex, mutex;
int numCars;
int rightCars, leftCars;
int carsOnBridge;
int currentDirection;

// functions for stimulating car crossing, waiting, arriving and leaving the bridge.
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

void waitingCar(int carId, int direction)
{
    if (direction == 1)
    {
        printf("Car %d (R) ---> ---> waiting.\n", carId);
    }
    else
    {
        printf("Car %d \t\t\t\t(L) <--- <--- waiting.\n", carId);
    }
}

void bridgeArrived(int carId, int direction)
{
    if (direction == 1)
    {
        printf("Car %d (R) ---> ---> arrived.\n", carId);
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
    bridgeArrived(carId, 1);
    // after arriving tries to aquire the emutex semaphore to enter the bridge
    // thus tracking the car arrived first by being aquired by that thread

    // is aquired by the car if there are no other cars waiting for their turn to cross the bridge
    // as if a car is not allowed to cross, it waits without releasing the mutex semaphore
    // thus preventing the cars arriving after the waiting car
    // hence, prevents starvation of the car on opposite side
    sem_wait(&emutex);
    while (1)
    {

        if (carsOnBridge < 3 && (currentDirection == 1 || currentDirection == 0))
        {
            sem_post(&emutex);
            sem_wait(&mutex);
            carsOnBridge++;
            currentDirection = 1;
            sem_post(&mutex);
            crossingBridge(carId, 1);

            sleep(2);

            sem_wait(&mutex);
            carsOnBridge--;
            leavingBridge(carId, 1);
            if (carsOnBridge == 0)
            {
                currentDirection = 0;
            }
            sem_post(&mutex);
            break;
        }
        else
        {
            waitingCar(carId, 1);
            sleep(1);
        }
    }
    pthread_exit(NULL);
}

void *leftCar(void *arg)
{
    int carId = *(int *)arg;
    bridgeArrived(carId, -1);
    sem_wait(&emutex);

    while (1)
    {
        if (carsOnBridge < 3 && (currentDirection == -1 || currentDirection == 0))
        {
            sem_post(&emutex);
            sem_wait(&mutex);
            carsOnBridge++;
            currentDirection = -1;
            sem_post(&mutex);
            crossingBridge(carId, -1);

            sleep(2);

            sem_wait(&mutex);
            carsOnBridge--;
            leavingBridge(carId, -1);
            if (carsOnBridge == 0)
                currentDirection = 0;
            sem_post(&mutex);
            break;
        }
        else
        {
            waitingCar(carId, -1);
            sleep(1);
        }
    }
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

    sem_init(&mutex, 1);
    sem_init(&emutex, 1);

    pthread_t cars[numCars];
    int threadArgs[numCars];

    for (int i = 0; i < numCars; i++)
    {
        threadArgs[i] = i + 1;
        int randomDirection = rand() % 2;
        if (randomDirection == 0 && rightCars > 0)
        {
            pthread_create(&cars[i], NULL, rightCar, (void *)&threadArgs[i]);
            rightCars--;
        }
        else if (randomDirection == 1 && leftCars > 0)
        {
            pthread_create(&cars[i], NULL, leftCar, (void *)&threadArgs[i]);
            leftCars--;
        }
    }

    for (int i = 0; i < numCars; i++)
    {
        pthread_join(cars[i], NULL);
    }

    sem_destroy(&emutex);
    sem_destroy(&mutex);

    return 0;
}
