#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

struct MeraSemaphore{

    unsigned int ValueOfSemaphore;
    pthread_cond_t ConditionVariable;
    pthread_mutex_t ConditionLock;

}Customer_Semaphore,Barber_Semaphore,Mutex_Semaphore;

void sem_wait(struct MeraSemaphore *s){
    pthread_mutex_lock(&(s->ConditionLock));
    while((s->ValueOfSemaphore) == 0){
        pthread_cond_wait(&(s->ConditionVariable), &(s->ConditionLock));
    }
    (s->ValueOfSemaphore)--;
    pthread_mutex_unlock(&(s->ConditionLock));
}

void sem_init(struct MeraSemaphore *s, int InitialValueOfSemaphore){
    pthread_cond_init(&(s->ConditionVariable), NULL);
    pthread_mutex_init(&(s->ConditionLock), NULL);
    s->ValueOfSemaphore = InitialValueOfSemaphore;
    return;
}

void sem_post(struct MeraSemaphore *s){
    pthread_mutex_lock(&(s->ConditionLock));
    (s->ValueOfSemaphore)++;
    pthread_cond_signal(&(s->ConditionVariable));
    pthread_mutex_unlock(&(s->ConditionLock));
}

void sem_destroy(struct MeraSemaphore *s){
    while(!(s->ValueOfSemaphore)){
        sem_post(s);
    }
    return;
}

#define NUM_OF_CHAIRS 7
#define HAIRCUT_TIME 2
#define NUM_OF_BARBERS 3
#define NUM_OF_CUSTOMERS 20
#define MOD 2000

int NumberOfFreeSeats=NUM_OF_CHAIRS;
int Seat_To_Customer_Map[NUM_OF_CHAIRS];
int Next_Seat_To_Be_Occupied=0;
int Chair_Index=0;
int Customer_Number=0;

void Barber_Thread(void *ptr){

    int index=*((int*)ptr);
    int Customer_ID=-1,Next_Customer_To_Be_Served;

    while(1){
        sem_wait(&Barber_Semaphore);
        sem_wait(&Mutex_Semaphore);
        Chair_Index=(Chair_Index+1)%NUM_OF_CHAIRS;
        Next_Customer_To_Be_Served=Chair_Index;
        Customer_ID=Seat_To_Customer_Map[Next_Customer_To_Be_Served];
        Seat_To_Customer_Map[Next_Customer_To_Be_Served]=pthread_self();
        sem_post(&Mutex_Semaphore);
        sem_post(&Customer_Semaphore);

        printf("Barber %d is cutting the hair of customer %d\n",index,Customer_ID);

        sleep(HAIRCUT_TIME);

        printf("Done with the haircut\n");
    }
    return;
}

void Customer_Thread(void *ptr){

    int Seat_Index, Barber_ID=-1;

    sem_wait(&Mutex_Semaphore);
    Customer_Number++;
    printf("Customer %d has entered the shop\n",Customer_Number);

    if(NumberOfFreeSeats<=0){
        printf("Customer %d has no free seats and hence leaves\n",Customer_Number);
        sem_post(&Mutex_Semaphore);
        pthread_exit(0);
        return;
    }

    NumberOfFreeSeats--;

    printf("Customer %d finds a seat in the shop and sits in the waiting room\n",Customer_Number);
    Next_Seat_To_Be_Occupied=(Next_Seat_To_Be_Occupied+1)%NUM_OF_CHAIRS;
    Seat_Index=Next_Seat_To_Be_Occupied;
    Seat_To_Customer_Map[Seat_Index]=Customer_Number;

    sem_post(&Mutex_Semaphore);
    sem_post(&Barber_Semaphore);
    sem_wait(&Customer_Semaphore);
    sem_wait(&Mutex_Semaphore);

    Barber_ID=Seat_To_Customer_Map[Seat_Index];
    NumberOfFreeSeats++;

    sem_post(&Mutex_Semaphore);

    pthread_exit(0);
    return;
}

int main(){
    pthread_t Barbers[NUM_OF_BARBERS];
    pthread_t Customers[NUM_OF_CUSTOMERS];


    sem_init(&Customer_Semaphore,0);
    sem_init(&Barber_Semaphore,0);
    sem_init(&Mutex_Semaphore,1);


    printf("Opening the Shop\n");
    for(int i=0;i<NUM_OF_BARBERS;i++){
        pthread_create(&Barbers[i],NULL,(void *)Barber_Thread,(void *)&i);
    }


    printf("Customers start coming\n");
    for(int i=0;i<NUM_OF_CUSTOMERS;i++){
        pthread_create(&Customers[i],NULL,(void *)Customer_Thread,(void *)&i);
    }

    for(int i=0;i<NUM_OF_CUSTOMERS;i++){
        pthread_join(Customers[i],NULL);
    }

    exit(0);
}
