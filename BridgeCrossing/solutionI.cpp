#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

pthread_mutex_t mutex;
pthread_mutex_t emutex;

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
    bridgeArrived(carId, 1);
    pthread_mutex_lock(&emutex);
    while (true)
    {

        if (carsOnBridge < 3 && (currentDirection == 1 || currentDirection == 0))
        {
            pthread_mutex_lock(&mutex);
            carsOnBridge++;
            carsToCross[1]--;

            currentDirection = 1;
            crossingBridge(carId, 1);
            pthread_mutex_unlock(&mutex);
            pthread_mutex_unlock(&emutex);

            sleep(2);

            pthread_mutex_lock(&mutex);
            carsOnBridge--;
            leavingBridge(carId, 1);
            if (carsOnBridge == 0)
            {
                currentDirection = 0;
            }
            pthread_mutex_unlock(&mutex);
            break;
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
            carsToCross[0]--;

            currentDirection = -1;
            crossingBridge(carId, -1);
            pthread_mutex_unlock(&mutex);
            pthread_mutex_unlock(&emutex);

            sleep(2);

            pthread_mutex_lock(&mutex);
            carsOnBridge--;
            leavingBridge(carId, -1);
            if (carsOnBridge == 0)
                currentDirection = 0;
            pthread_mutex_unlock(&mutex);
            break;
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

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&emutex, NULL);

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
