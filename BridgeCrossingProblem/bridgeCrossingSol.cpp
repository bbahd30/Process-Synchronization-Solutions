#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

sem_t bridgeSem;
pthread_mutex_t mutex;

int numCars;
int carsOnBridge;
int currentDirection;

void crossingBridge(int carId, int direction)
{
    if (direction == 1)
        cout << "Car " << carId << " is heading towards right\n";
    else
        cout << "Car " << carId << " is heading towards left\n";

    sleep(2);
    // so that the other cars can also check for crossing
}

void waitingCar(int carId, int direction)
{
    if (direction == 1)
        cout << "Car " << carId << " heading towards right, couldn't cross bridge and waiting.\n";
    else
        cout << "Car " << carId << " heading towards left, couldn't cross bridge and waiting.\n";

    sleep(2);
}

void bridgeArrived(int carId, int direction)
{
    if (direction == 1)
        cout << "Car " << carId << " heading towards right arrived.\n";
    else
        cout << "Car " << carId << " heading towards left arrived.\n";
}

void leavingBridge(int carId, int direction)
{
    if (direction == 1)
        cout << "Car " << carId << " is leaving bridge from right\n";
    else
        cout << "Car " << carId << " is leaving bridge from left\n";
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

            sem_wait(&bridgeSem);
            sleep(1);
            sem_post(&bridgeSem);

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
            crossingBridge(carId, -1); // to make the print statement atomic as will cross the bridge, satisfied
            pthread_mutex_unlock(&mutex);

            sem_wait(&bridgeSem);
            sleep(1);
            sem_post(&bridgeSem);

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

    sem_init(&bridgeSem, 0, 3);
    pthread_mutex_init(&mutex, NULL);

    pthread_t cars[numCars];
    int threadArgs[numCars];

    for (int i = 0; i < numCars; i++)
    {
        threadArgs[i] = i + 1;
    }

    for (int i = 0; i < rightCars; i++)
    {
        pthread_create(&cars[i], NULL, rightCar, (void *)&threadArgs[i]);
    }

    for (int i = rightCars; i < numCars; i++)
    {
        pthread_create(&cars[i], NULL, leftCar, (void *)&threadArgs[i]);
    }

    for (int i = 0; i < numCars; i++)
    {
        pthread_join(cars[i], NULL);
    }
    return 0;
}
