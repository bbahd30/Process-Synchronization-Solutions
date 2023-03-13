#include <fcntl.h>
#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#define totalCustomers 18

using namespace std;

int numberOfCustomersEating = 0, numberOfCustomersWaiting = 0;
sem_t *mutex1, *lock1, *mutex2;
bool shouldCustomersWait = false;

void* customerBehavior(void* args) {
	sem_wait(mutex1);
	sleep(0.001);
	if (shouldCustomersWait) {
		numberOfCustomersWaiting += 1;
		sem_post(mutex1);
		// print
		printf("Customer %d is waitng\n", (*(int*)args + 1));
		sem_wait(lock1);
		// sem_post(mutex2);
		// sem_wait(lock1);
	}else {
		numberOfCustomersEating += 1;
		shouldCustomersWait = (numberOfCustomersEating == 5);
		sem_post(mutex1);
		// sem_wait(mutex2);
		// sem_post(mutex2);
	}

	// time to east sushi
	printf("Customer %d is eating\n", (*(int*)args + 1));
	sleep(0.1);
	
    sem_wait(mutex2);
	numberOfCustomersEating -= 1;
	// sem_wait(mutex2);
	printf("Customer %d is leaving\n", (*(int*)args + 1));
	// sem_post(mutex2);
	if (numberOfCustomersEating == 0) {
		int x;
		if (numberOfCustomersWaiting < 5) {
			x = numberOfCustomersWaiting;
		}else x = 5;
		numberOfCustomersWaiting -= x;
		numberOfCustomersEating += x;
		shouldCustomersWait = (numberOfCustomersEating == 5);
		for (int i = 0; i < x; i++) {
			sem_post(lock1);
		}
	}
	sem_post(mutex2);
    return NULL;
}

int main() {
	pthread_t arrayOfThreadsForEachCustomer[totalCustomers];
	mutex2 = sem_open("mutex2",O_CREAT, S_IRUSR|S_IWUSR, 1);
    mutex1 = sem_open("mutex1",O_CREAT, S_IRUSR|S_IWUSR, 1);
    lock1 = sem_open("lock1",O_CREAT, S_IRUSR|S_IWUSR, 0);

    sem_unlink("mutex2");
    sem_unlink("mutex1");
    sem_unlink("lock1");
	for (int i = 0; i < totalCustomers; i++) {
        int* a = (int*)malloc(sizeof(int));
        *a = i;
		pthread_create(&arrayOfThreadsForEachCustomer[i], NULL, customerBehavior, a);
	}
	for (int i = 0; i < totalCustomers; i++) {
		pthread_join(arrayOfThreadsForEachCustomer[i], NULL);
	}
	return 0;
}

