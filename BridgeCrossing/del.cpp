#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
using namespace std;

sem_t bridgeSem;
pthread_mutex_t mutex;

int numCars;
int rightCars, leftCars;
int carsOnBridge;
int currentDirection;
//
int carsToCross[2];

void crossingBridge(int carId, int direction)
{
    string directionVar = (direction == 1) ? "(R) ---> --->" : "\t\t\t\t(L) <--- <---";
    cout << "Car " << carId << " " << directionVar << " crossing.\n";
}

void waitingCar(int carId, int direction)
{
    string directionVar = (direction == 1) ? "(R)" : "\t\t\t\t(L)";
    cout << "Car " << carId << " " << directionVar << " waiting.\n";
}

void bridgeArrived(int carId, int direction)
{
    string directionVar = (direction == 1) ? "(R)" : "\t\t\t\t(L)";
    cout << "Car " << carId << " " << directionVar << " arrived.\n";
}

void leavingBridge(int carId, int direction)
{
    string directionVar = (direction == 1) ? "(R) ---> --->" : "\t\t\t\t(L) <--- <---";
    cout << "Car " << carId << " " << directionVar << " leaving.\n";
}

void *rightCar(void *arg)
{
    int carId = *(int *)arg;

    pthread_mutex_lock(&mutex);
    bridgeArrived(carId, 1);
    while (true)
    {
        if (((currentDirection == 1 || currentDirection == 0) && carsOnBridge < 3))
        {
            currentDirection = 1;
            carsOnBridge++;
            carsToCross[1]--;
            crossingBridge(carId, 1);
            pthread_mutex_unlock(&mutex);

            sem_wait(&bridgeSem);
            sleep(2);
            sem_post(&bridgeSem);

            pthread_mutex_lock(&mutex);
            carsOnBridge--;
            leavingBridge(carId, 1);
            // after crossing the bridge need to take care if the last one on bridge, then have to check check both the sides
            // to prevent starvation, should look the number of cars on the other side waiting, if no one of left, then right-moving cars move
            if (carsOnBridge == 0 && carsToCross[0] > 0)
                currentDirection = -1;
            pthread_mutex_unlock(&mutex);
            break;
        }
        else
        {
            waitingCar(carId, 1);
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }
    pthread_exit(NULL);
}

void *leftCar(void *arg)
{
    int carId = *(int *)arg;
    bridgeArrived(carId, -1);

    while (true)
    {
        pthread_mutex_lock(&mutex);
        if (((currentDirection == -1 || currentDirection == 0) && carsOnBridge < 3))
        {
            currentDirection = -1;
            carsOnBridge++;
            crossingBridge(carId, -1); // to make the print statement atomic as will cross the bridge, satisfied
            pthread_mutex_unlock(&mutex);

            sem_wait(&bridgeSem);
            sleep(1);
            sem_post(&bridgeSem);

            pthread_mutex_lock(&mutex);
            carsOnBridge--;
            leavingBridge(carId, -1);
            if (carsOnBridge == 0 && carsToCross[1] > 0)
                currentDirection = 1;
            pthread_mutex_unlock(&mutex);
            break;
        }
        else
        {
            waitingCar(carId, -1);
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }
    pthread_exit(NULL);
}

int main()
{
    cout << "Enter the number of cars: ";
    cin >> numCars;
    cout << "Enter the number of cars heading towards left: ";
    cin >> leftCars;
    rightCars = numCars - leftCars;

    carsToCross[0] = leftCars;
    carsToCross[1] = rightCars;

    currentDirection = 0;
    carsOnBridge = 0;

    sem_init(&bridgeSem, 0, 3);
    pthread_mutex_init(&mutex, NULL);

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
    return 0;
}

// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <stdatomic.h>
// #include "../Semaphore.h"

// struct Semaphore emutex, mutex, left_sem, right_sem;
// int numCars;
// int rightCars, leftCars;
// int carsOnBridge;
// int currentDirection;

// void *rightCar(void *arg)
// {
//     int carId = *(int *)arg;
//     bridgeArrived(carId, 1);
//     sem_wait(&emutex);
//     sem_wait(&right_sem);

//     while (1)
//     {
//         sem_wait(&mutex);
//         if (carsOnBridge < 3 && sem_getvalue(&left_sem) == 1)
//         {
//             carsOnBridge++;
//             currentDirection = 1;
//             sem_post(&mutex);
//             sem_post(&right_sem);
//             crossingBridge(carId, 1);

//             sleep(2);

//             sem_wait(&mutex);
//             carsOnBridge--;
//             leavingBridge(carId, 1);
//             if (carsOnBridge == 0)
//             {
//                 currentDirection = 0;
//             }
//             sem_post(&mutex);
//             break;
//         }
//         else
//         {
//             sem_post(&mutex);
//             waitingCar(carId, 1);
//             sleep(1);
//         }
//     }
//     pthread_exit(NULL);
// }

// void *leftCar(void *arg)
// {
//     int carId = *(int *)arg;
//     bridgeArrived(carId, -1);
//     sem_wait(&emutex);
//     sem_wait(&left_sem);

//     while (1)
//     {
//         sem_wait(&mutex);
//         if (carsOnBridge < 3 && sem_getvalue(&right_sem) == 1)
//         {
//             carsOnBridge++;
//             currentDirection = -1;
//             sem_post(&mutex);
//             sem_post(&left_sem);
//             crossingBridge(carId, -1);

//             sleep(2);

//             sem_wait(&mutex);
//             carsOnBridge--;
//             leavingBridge(carId, -1);
//             if (carsOnBridge == 0)
//             {
//                 currentDirection = 0;
//             }
//             sem_post(&mutex);
//             break;
//         }
//         else
//         {
//             sem_post(&mutex);
//             waitingCar(carId, -1);
//             sleep(1);
//         }
//     }
//     pthread_exit(NULL);
// }

// int main()
// {
//     printf("Enter the number of cars: ");
//     scanf("%d", &numCars);
//     printf("Enter the number of cars heading towards left: ");
//     scanf("%d
