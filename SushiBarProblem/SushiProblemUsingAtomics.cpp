#include<unistd.h>
#include <bits/stdc++.h>
#include<pthread.h>
#include<atomic>

using namespace std;

#define totalCustomers 18

typedef struct{
    volatile atomic_int value;
    volatile atomic_flag mutex;

}Merasemaphore;

void sem_wait(Merasemaphore *s) {
    while(atomic_flag_test_and_set(&s->mutex));
    while(atomic_load(&s->value)<=0);
    atomic_fetch_sub(&s->value, 1);
    atomic_flag_clear(&s->mutex);
}

void sem_init(Merasemaphore *s, int intialValue) {
      atomic_init(&s->value, intialValue);
}

void sem_post(Merasemaphore *s) {
        atomic_fetch_add(&s->value,1);
}

void sem_destroy(Merasemaphore *s){
    while(!(s->value))sem_post(s);
}


// g++ SushiProblemUsingAtomics.cpp -o ex -lpthread
// ./ex

int numberOfCustomersEating = 0, numberOfCustomersWaiting = 0;
Merasemaphore mutex1, mutex2, lock1;
bool shouldCustomersWait = false;

void* customerBehavior(void* args) {
	sleep(2);
	sem_wait(&mutex1);
	//entering Sushi bar
	if (shouldCustomersWait) {
		numberOfCustomersWaiting += 1;
		sem_post(&mutex1);
		// print
		printf("Customer %d is waitng\n", (*(int*)args + 1));
		sem_wait(&lock1);
		//blocking the customer until the table is empty
	}else {
		numberOfCustomersEating += 1;
		shouldCustomersWait = (numberOfCustomersEating == 5);
		//updating the boolean variable
		sem_post(&mutex1);
	}

	// time to east sushi
	printf("Customer %d is eating\n", (*(int*)args + 1));
	sleep(12);
	
    sem_wait(&mutex2);
    //leaving Sushi bar
	numberOfCustomersEating -= 1;
	printf("Customer %d is leaving\n", (*(int*)args + 1));
	if (numberOfCustomersEating == 0) {
		int x = min(5, numberOfCustomersWaiting);
		// x denotes the number of customers that will be eating
		// at the next round
		numberOfCustomersWaiting -= x;
		numberOfCustomersEating += x;
		shouldCustomersWait = (numberOfCustomersEating == 5);
		// updating the boolean variable
		//sem_post(&lock1, x);
		// the above command results into deadlock, so has been commented out
		for (int i = 0; i < x; i++) {
			sem_post(&lock1);
		}
		// signalling the next x customers to enter the critical
		// section aka sushi bar
	}
	sem_post(&mutex2);
    return NULL;
}

int main() {
	pthread_t arrayOfThreadsForEachCustomer[totalCustomers];
	sem_init(&mutex1, 1);
	// intializing mutex1 to 1
    sem_init(&mutex2, 1);
    // intializing mutex2 to 1
    sem_init(&lock1, 0);
    // intializing lock1 to 0

	for (int i = 0; i < totalCustomers; i++) {
        int* a = (int*)malloc(sizeof(int));
        *a = i;
		pthread_create(&arrayOfThreadsForEachCustomer[i], NULL, customerBehavior, a);
		// creating a thread for each customer
	}
	for (int i = 0; i < totalCustomers; i++) {
		pthread_join(arrayOfThreadsForEachCustomer[i], NULL);
	}

    sem_destroy(&mutex2);
    sem_destroy(&mutex1);
    sem_destroy(&lock1);
    //destroying each semaphore at the end
	return 0;
}

