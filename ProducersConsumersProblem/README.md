# Producer Consumer Problem 
###  Problem Statement
It is a classical multi-process synchronisation problem. It has two types of processes which share a common buffer. The processes are explained below: <br>
1. Producer : Generates data and adds it to the buffer if the buffer has space available in it.<br>
2. Consumer : The consumer consumes the data if the buffer is not empty, thus deleting it from the buffer. <br>
<br>
Three types of buffer are explained in this solution <br>
1. Bounded Buffer <br>
2. Cyclic Buffer <br>
3. Unbounded Buffer <br>

### Solution 
#### Bounded:
The buffer is bounded i.e. it has a defined capacity, if the producer produces objects greater than it's capacity then it has to wait for objects to be consumed inorder to produce more. The major semaphores used are not_full, not_empty and mutex. The `not_full` semaphore indicates that the buffer hasn't reached it's capacity. The `not_empty` semaphore indicates that the buffer isn't empty and `mutex` ensures mutual exclusion for critical section.
###### Producer and Consumer functions:
```c
void Producer_Process(void *producer_no)
{
    int object;
    for(int i=0;i<Max;i++)
    {
        sem_wait(&not_full);
        if(in==SizeofBuffer) {
        	in=0;
        }
        object = rand(); 
        sem_wait(&mutex);
        buffer[in] = object;
        printf("Producer %d Inserted object %d at %d \n",*((int *)producer_no), buffer[in],in);
        in = in+1;
        sem_post(&mutex);
        sem_post(&not_empty); 
    }
}

void Consumer_Process(void *consumer_no)
{
    for(int i=0;i<Max;i++)
    {
        sem_wait(&not_empty);
        if(out==SizeofBuffer)
        {
        	out=0;
        }
        sem_wait(&mutex);
        int object = buffer[out];
        printf("Consumer %d Removed object %d at %d \n",*((int *)consumer_no), buffer[out],out);
        buffer[out] = -1;
        out = out+1;
        sem_post(&mutex);
        sem_post(&not_full);
    }
}
```
#### Cyclic:
In this case the buffer is implemented such that the producer never waits, if the buffer is full it will start replacing the elements from the start. The consumer has to wait if the buffer is empty otherwise it can consume. Here we do not need `not_full` semaphore.
###### Producer and Consumer functions:
```c
void Producer_Process(void *producer_no)
{
    int object;
    for(int i=0;i<Max;i++)
    {
        object = rand(); 
        // pthread_mutex_lock(&mutex);
        sem_wait(&mutex);
        buffer[in] = object;
        printf("Producer %d Inserted object %d at %d \n",*((int *)producer_no), buffer[in],in);
        in = in+1;
        in = in%SizeofBuffer;
        sem_post(&mutex);
        sem_post(&not_empty);    
    }
}


void Consumer_Process(void *consumer_no)
{
    for(int i=0;i<Max;i++)
    {
        sem_wait(&not_empty);
        sem_wait(&mutex);
        int object = buffer[out];
        printf("Consumer %d Removed object %d at %d \n",*((int *)consumer_no), buffer[out],out);
        buffer[out] = -1;
        out = out+1;
        out = out%SizeofBuffer;
        sem_post(&mutex);
    }
}
```

#### Unbounded Buffer 
In this case there is no limit on the size of the buffer. Thus, in this case the `not_full` semaphore used in the bounded buffer case is not required. In this case the producer never waits. `mutex` semphore ensures mutual exclusion of the processes.
We have implemented a linked list structure for implementing the infinite buffer.
###### Unbounded buffer structure implementation:
```c
struct node
{
        int value;
        struct node *next;
};

void add_to_buffer(int value)
{
    //adds the newly produced object
}

void remove_from_buffer()
{
    //removes from buffer
}

void display()
{
    //displays all the elements
}
```
###### Producer and Consumer functions:
```c
void Producer_Process(void *producer_no)
{
    int object;
    for(int i=0;i<Max;i++)
    { 
        object = rand(); 
        sem_wait(&mutex);
        insert_end(object);
        printf("Producer %d i: %d Inserted object %d\n",*((int *)producer_no),i,object);
        sem_post(&mutex);
        int val;
        sem_post(&not_empty); 
    }
}

void Consumer_Process(void *consumer_no)
{
	
    for(int i=0;i<Max;i++)
    {
        sem_wait(&not_empty);
        sem_wait(&mutex);
        int object;
        object = delete_begin();
        printf("Consumer %d i: %d Removed object %d\n",*((int *)consumer_no),i, object);    
        sem_post(&mutex);
    }
}
```
