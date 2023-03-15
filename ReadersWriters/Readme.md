# Readers Writers Synchronization Problem

## Problem Description:

Readers-Writers problem is a classical problem involving sharing of a memory or database by multiple processes concurrently. Readers read data from a shared location and writers write data to that shared location. Any number of readers can access the critical section simultaneously but only one writer can access the critical section at one point of time. While accessing the critical section, there could be synchronization problems. Thus, semaphores are used to solve those problems.

## Solution

Pthreads have been used to show the simultaneous occurrence of readers and writers processes. Each reader and writer is represented using a pthread. While readers can occur simultaneously, no writer is allowed to execute till all readers are done. So mutual exclusion is necessary among readers and writers. Our solution uses three semaphores: `order`, `access_mutex`, `read_mutex`.

- `order` : This is used to maintain entry of processes and to ensure FIFO method.
- `access_mutex` : This is used to ensure that only one process (reader or writer) accesses the critical section at one time. (ensures mutual exclusion)
- `read_mutex` : This is used in updation of readerscount.

```c
void *reader_function(void*rid){

    int i=*((int *)rid);
    sem_wait(&order);
    sem_wait(&read_mutex);

    readerscount++;

    if(readerscount==1){
        sem_wait(&access_mutex);
    }

    sem_post(&read_mutex);
    sem_post(&order);

    /*critical section*/
    printf("Reader %i is reading data %i \n",i,shared_data);

    sem_wait(&read_mutex);
    readerscount--;
    sem_post(&read_mutex);
    if(readerscount==0){
        sem_post(&access_mutex);
    }

    return 0;
}

void *writer_function(void *wid){

    int i=*((int *)wid);

    sem_wait(&order);
    sem_wait(&access_mutex);

    sem_post(&order);

    /*critical section*/

    shared_data+=10;
    printf("writer %i is writing data= %i \n",i,shared_data);


    sem_post(&access_mutex);

    return 0;
}

```

Here, next process is allowed entry only when initial reader/writer process has already begun its execution and is in critical section. `access_mutex` ensures that at once either a reader or a writer accesses critical section. Once reader/writer frees critical section only then a another reader/writer can access it. This gives no priority to any process and thus allows starvation and deadlock free execution.
