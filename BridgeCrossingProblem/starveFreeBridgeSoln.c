#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

struct Merasemaphore
{
    volatile atomic_int value;
    volatile atomic_flag mutex;

} emutex,mutex;

void sem_wait(struct Merasemaphore *s)
{
    while (atomic_flag_test_and_set(&s->mutex))
        ;
    while (atomic_load(&s->value) <= 0)
        ;
    atomic_fetch_sub(&s->value, 1);
    atomic_flag_clear(&s->mutex);
}

void sem_init(struct Merasemaphore *s, int value)
{
    atomic_init(&s->value, value);
}

void sem_post(struct Merasemaphore *s)
{
    atomic_fetch_add(&s->value, 1);
}

void sem_destroy(struct Merasemaphore *s){
    while(!(s->value))sem_post(s);
}


int numCars;
int rightCars, leftCars;
int carsOnBridge;
int currentDirection;

void crossingBridge(int carId, int direction)
{
    if(direction==1){
        printf("Car %d (R) ---> ---> crossing.\n",carId);
    }
    else{
        printf("Car %d \t\t\t\t(L) <--- <--- crossing.\n",carId);
    }
}

void waitingCar(int carId, int direction)
{
    if(direction==1){
        printf("Car %d (R) ---> ---> waiting.\n",carId);
    }
    else{
        printf("Car %d \t\t\t\t(L) <--- <--- waiting.\n",carId);
    }
}

void bridgeArrived(int carId, int direction)
{
    if(direction==1){
        printf("Car %d (R) ---> ---> arrived.\n",carId);
    }
    else{
        printf("Car %d \t\t\t\t(L) <--- <--- arrived.\n",carId);
    }
}

void leavingBridge(int carId, int direction)
{
    if(direction==1){
        printf("Car %d (R) ---> ---> leaving.\n",carId);
    }
    else{
        printf("Car %d \t\t\t\t(L) <--- <--- leaving.\n",carId);
    }
}

void *rightCar(void *arg)
{
    int carId = *(int *)arg;
    bridgeArrived(carId, 1);
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
    scanf("%d",&numCars);
    printf("Enter the number of cars heading towards left: ");
    scanf("%d",&leftCars);


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

    sem_destroy(&emutex);
    sem_destroy(&mutex);

    return 0;
}
