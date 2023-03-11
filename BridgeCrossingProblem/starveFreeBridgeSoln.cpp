#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
using namespace std;

sem_t bridgeSem;
pthread_mutex_t mutex;
pthread_mutex_t emutex;

int numCars;
int rightCars, leftCars;
int carsOnBridge;
int currentDirection;

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
    bridgeArrived(carId, 1);
    pthread_mutex_lock(&emutex);
    while (true)
    {

        if (carsOnBridge < 3 && (currentDirection == 1 || currentDirection == 0))
        {
            pthread_mutex_lock(&mutex);
            carsOnBridge++;
            currentDirection = 1;
            crossingBridge(carId, 1);
            pthread_mutex_unlock(&mutex);
            pthread_mutex_unlock(&emutex);

            sem_wait(&bridgeSem);
            sleep(2);
            sem_post(&bridgeSem);

            pthread_mutex_lock(&mutex);
            carsOnBridge--;
            leavingBridge(carId, 1);
            // after crossing the bridge need to take care if the last one on bridge, then have to check both the sides
            // to prevent starvation, should look the number of cars on the other side waiting, if no one of left, then right-moving cars move
            if (carsOnBridge == 0)
            {
                currentDirection = 0;
            }
            pthread_mutex_unlock(&mutex);
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
    pthread_mutex_lock(&emutex);

    while (true)
    {
        if (carsOnBridge < 3 && (currentDirection == -1 || currentDirection == 0))
        {
            pthread_mutex_lock(&mutex);
            carsOnBridge++;
            currentDirection = -1;
            crossingBridge(carId, -1);
            pthread_mutex_unlock(&mutex);
            pthread_mutex_unlock(&emutex);

            sem_wait(&bridgeSem);
            sleep(2);
            sem_post(&bridgeSem);

            pthread_mutex_lock(&mutex);
            carsOnBridge--;
            leavingBridge(carId, -1);
            // after crossing the bridge need to take care if the last one on bridge, then have to check both the sides
            // to prevent starvation, should look the number of cars on the other side waiting, if no one of left, then right-moving cars move
            if (carsOnBridge == 0)
                currentDirection = 0;
            pthread_mutex_unlock(&mutex);
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
    cout << "Enter the number of cars: ";
    cin >> numCars;
    cout << "Enter the number of cars heading towards left: ";
    cin >> leftCars;
    rightCars = numCars - leftCars;

    currentDirection = 0;
    carsOnBridge = 0;

    sem_init(&bridgeSem, 0, 3);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&emutex, NULL);

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
