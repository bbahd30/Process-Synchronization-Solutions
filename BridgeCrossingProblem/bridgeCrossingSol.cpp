#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

pthread_mutex_t mutex;

int numCars;
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
    pthread_mutex_lock(&mutex);
    int carId = *(int *)arg;
    bridgeArrived(carId, 1);
    while (true)
    {
        if (((currentDirection == 1 || currentDirection == 0) && carsOnBridge < 3) || ((currentDirection == -1) && carsOnBridge == 0))
        {
            currentDirection = 1;
            carsOnBridge++;
            crossingBridge(carId, 1);
            pthread_mutex_unlock(&mutex);

            sleep(1);

            pthread_mutex_lock(&mutex);
            carsOnBridge--;
            leavingBridge(carId, 1);
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
        if (((currentDirection == -1 || currentDirection == 0) && carsOnBridge < 3) || ((currentDirection == 1) && carsOnBridge == 0))
        {

            currentDirection = -1;
            carsOnBridge++;
            crossingBridge(carId, -1);
            pthread_mutex_unlock(&mutex);

            sleep(1);

            pthread_mutex_lock(&mutex);
            carsOnBridge--;
            leavingBridge(carId, -1);
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
    int rightCars, leftCars;
    cout << "Enter the number of cars heading towards left: ";
    cin >> leftCars;

    rightCars = numCars - leftCars;

    currentDirection = 0;
    carsOnBridge = 0;

    pthread_mutex_init(&mutex, NULL);

    pthread_t cars[numCars];
    int threadArgs[numCars];

    // for (int i = 0; i < numCars; i++)
    // {
    //     threadArgs[i] = i + 1;
    // }
    for (int i = 0; i < numCars; i++)
    {
        threadArgs[i] = i + 1;
        if (rand() % 2 == 0 && rightCars > 0)
        {
            pthread_create(&cars[i], NULL, rightCar, (void *)&threadArgs[i]);
            rightCars--;
        }
        else
        {
            pthread_create(&cars[i], NULL, leftCar, (void *)&threadArgs[i]);
        }
    }

    // for (int i = 0; i < rightCars; i++)
    // {
    //     pthread_create(&cars[i], NULL, rightCar, (void *)&threadArgs[i]);
    // }

    // for (int i = rightCars; i < numCars; i++)
    // {
    //     pthread_create(&cars[i], NULL, leftCar, (void *)&threadArgs[i]);
    // }

    for (int i = 0; i < numCars; i++)
    {
        pthread_join(cars[i], NULL);
    }
    return 0;
}
