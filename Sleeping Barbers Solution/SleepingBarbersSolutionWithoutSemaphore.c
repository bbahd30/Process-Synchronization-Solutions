#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<stdatomic.h>

#define NUMBER_OF_CHAIRS 8
#define HAIRCUT_TIME 2
#define NUMBER_OF_BARBERS 2
#define NUMBER_OF_CUSTOMERS 20
#define MOD 800000

struct Merasemaphore{
    volatile atomic_int value;
    volatile atomic_flag mutex;

}BarberSleep,WaitingCustomer,AccessWaitChairs,BarberSemaphore[NUMBER_OF_BARBERS];

void sem_wait(struct Merasemaphore *s){
    while(atomic_flag_test_and_set(&s->mutex));
    while(atomic_load(&s->value)<=0);
    atomic_fetch_sub(&s->value, 1);
    atomic_flag_clear(&s->mutex);
}

void sem_init(struct Merasemaphore *s, int value){
    atomic_init(&s->value,value);
}

void sem_post(struct Merasemaphore *s){
    atomic_fetch_add(&s->value,1);
}

void sem_destroy(struct Merasemaphore *s){
    while(!(s->value))sem_post(s);
}

int RandomNumberGenerator(){
    int x=rand()%MOD+100;
    srand(time(NULL));
    return x;
}

void Wait_Before_Next_Customer_Arrives(){
    usleep(RandomNumberGenerator());
}

int ChairIndexOfFirstWaitingCustomer;
int SeatToCustomerMap[NUMBER_OF_CHAIRS];
int NumberOfFreeWaitingChairs;
int NextSeatToBeOccupied;

void BarberThread(void *ptr){
    int Index=*((int *)ptr)+1;
    int CustomerID;             // ID of the customer that the barber will serve
    int NextCustomerToBeServed; //index of the chair where next customer will be served

    while(1){

        sem_wait(&WaitingCustomer);
        sem_wait(&AccessWaitChairs);

        CustomerID=SeatToCustomerMap[ChairIndexOfFirstWaitingCustomer];
        ChairIndexOfFirstWaitingCustomer=(ChairIndexOfFirstWaitingCustomer+1)%NUMBER_OF_CHAIRS;

        sem_post(&AccessWaitChairs);
        sem_post(&BarberSleep);

        sem_wait(&BarberSemaphore[Index-1]);
        NumberOfFreeWaitingChairs++;
        printf("Barber %d cuts the hair of the customer %d\n",Index,CustomerID);
        sleep(HAIRCUT_TIME);        
        sem_post(&BarberSemaphore[Index-1]);
    }
    pthread_exit(0);
    return;
}

void CustomerThread(void *ptr){

    int Index=*((int *)ptr)+1;         //ID of the customer

    sem_wait(&AccessWaitChairs);
    
    printf("Customer %d has entered the shop\n",Index);

    if(NumberOfFreeWaitingChairs<=0){

        printf("Customer %d finds no free seats in the shop and hence leaves\n",Index);
        sem_post(&AccessWaitChairs);

        pthread_exit(0);
        return;
    }

    printf("Customer %d sits in the chair %d inside the shop\n",Index,NextSeatToBeOccupied+1);
    NumberOfFreeWaitingChairs--;
    SeatToCustomerMap[NextSeatToBeOccupied]=Index;
    NextSeatToBeOccupied=(NextSeatToBeOccupied+1)%NUMBER_OF_CHAIRS;

    sem_post(&AccessWaitChairs);
    sem_post(&WaitingCustomer);
    sem_wait(&BarberSleep);

    /*      GET A HAIRCUT         */
}


int main(){

    pthread_t Barbers[NUMBER_OF_BARBERS];
    pthread_t Customers[NUMBER_OF_CUSTOMERS];

    for(int i=0;i<NUMBER_OF_BARBERS;i++){
        sem_init(&BarberSemaphore[i],1);
    }
    sem_init(&BarberSleep,0);
    sem_init(&WaitingCustomer,0);
    sem_init(&AccessWaitChairs,1);

    NextSeatToBeOccupied=0;
    NumberOfFreeWaitingChairs=NUMBER_OF_CHAIRS;
    ChairIndexOfFirstWaitingCustomer=0;

    printf("Opening the shop\n");

    for(int i=0;i<NUMBER_OF_BARBERS;i++){
        pthread_create(&Barbers[i],NULL,(void *)BarberThread,(void *)&i);
    }

    printf("Customers start coming to the shop\n");

    for(int i=0;i<NUMBER_OF_CUSTOMERS;i++){
        pthread_create(&Customers[i],NULL,(void *)CustomerThread,(void *)&i);
        Wait_Before_Next_Customer_Arrives();
    }

    for(int i=0;i<NUMBER_OF_CUSTOMERS;i++){
        pthread_join(Customers[i],NULL);
    }

    printf("Closing the shop\n");

    exit(0);
    
}
