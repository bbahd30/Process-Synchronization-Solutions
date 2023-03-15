#include <pthread.h>
#include "../Semaphore.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define Max 6
struct Semaphore not_empty;

struct node
{
        int value;
        struct node *next;
};


struct node *buffer;
struct node *end;

void display()
{
        struct node *ptr;
        if(buffer==NULL)
        {
                printf("List is empty%d \n",buffer->value);
                return;
        }
        else
        {
                ptr=buffer;
                printf("The List elements are\n");
                while(ptr!=NULL)
                {
                        printf("%d \n",ptr->value );
                        ptr=ptr->next ;
                }
        }
}
void add_to_buffer(int value)
{
        struct node *temp;
        temp=(struct node *)malloc(sizeof(struct node));
        if(temp==NULL)
        {
                printf("Out of Memory Space:n\n");
                return;
        }
        temp->value=value;
        temp->next =NULL;
        if(buffer==NULL)
        {
                buffer=temp;
                end = temp;
        }
        else
        {
                end->next = temp;
                end = temp;
        }
}

int remove_from_buffer()
{
        struct node *ptr;
        if(buffer==NULL)
        {
                printf("List is Empty: \n");
                return -1;
        }
        else
        {
        	int val;
                ptr=buffer;
                buffer=buffer->next ;
                val=ptr->value;
                free(ptr);
                return val;
                
        }
}

struct Semaphore mutex;

void Producer_Process(void *producer_no)
{
    int object;
    for(int i=0;i<Max;i++)
    {
        srand(i*Max+*((int *)producer_no));
        object = rand(); 
        sem_wait(&mutex);
        add_to_buffer(object);
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
        object = remove_from_buffer();
        printf("Consumer %d i: %d Removed object %d\n",*((int *)consumer_no),i, object);
        sem_post(&mutex);
        
    }
}

int main()
{
        int n=10;
        pthread_t producers[n];
        pthread_t consumers[n];
        sem_init(&mutex,1);
        sem_init(&not_empty, 0);
        int nos[n];
        for(int i=0;i<n;i++) nos[i]=i+1;
        for(int i = 0; i < n; i++) {
        pthread_create(&producers[i], NULL, (void *)&Producer_Process, (void *)&nos[i]);
        }
    
        for(int i = 1; i < n; i++) {
        pthread_create(&consumers[i], NULL, (void *)&Consumer_Process, (void *)&nos[i]);
        }

        for(int i = 0; i < n; i++) {
        pthread_join(producers[i], NULL);
        }

        for(int i = 1; i < n; i++) {
        pthread_join(consumers[i], NULL);
        }
	display();

        sem_destroy(&mutex);
        sem_destroy(&not_empty);

    return 0;
}