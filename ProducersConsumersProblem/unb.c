#include <pthread.h>
#include "../Semaphore.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define Max 6
// #define SizeofBuffer 10
struct Semaphore not_full;
struct Semaphore not_empty;

// FILE *fp;

struct node
{
        int info;
        struct node *next;
};


struct node *buffer;
struct node *end;

void display()
{
        struct node *ptr;
        // fp = fopen("Output.txt", "a+");
        if(buffer==NULL)
        {
                printf("nList is empty:n %d \n",buffer->info);
                return;
        }
        else
        {
                ptr=buffer;
                printf("The List elements are\n");
                while(ptr!=NULL)
                {
                        printf("%d \n",ptr->info );
                        ptr=ptr->next ;
                }
        }
        // fclose(fp);
}
void insert_end(int value)
{
	// printf("inside insert\n");
        // last->next=NULL;
        // end->next=last;
        // end=last;
        struct node *temp;
        temp=(struct node *)malloc(sizeof(struct node));
        if(temp==NULL)
        {
                // fp = fopen("Output.txt", "a+");
                printf("nOut of Memory Space:n\n");
                // fclose(fp);
                return;
        }
        temp->info=value;
        temp->next =NULL;
        if(buffer==NULL)
        {
                buffer=temp;
        }
        else
        {
                temp->next=buffer;
                buffer=temp;
        }
}

int delete_begin()
{
        struct node *ptr;
        if(buffer==NULL)
        {
                // fp = fopen("Output.txt", "a+");
                printf("nList is Empty:n\n");
                // fclose(fp);
                return -1;
        }
        else
        {
        	int val;
                ptr=buffer;
                buffer=buffer->next ;
                // fp = fopen("Output.txt", "a+");
                printf("nThe deleted element is :%dt\n",ptr->info);
                // fclose(fp);
                val=ptr->info;
                free(ptr);
                return val;
                // int val = buffer->info;
                // printf("deleted ele is: %d",buffer->info);
                // buffer=buffer->next;
                // return val;
        }
}

// int main()
// {
// 	printf("hhh");
// 	struct node* temp;
// 	printf("jjj");
// 	temp->info=76;
// 	printf("%d",temp->info);
// 	int value=79;
// 	insert_end(value);
// 	display();
// 	struct node* temp2;
// 	temp2->info=76;
// 	insert_end(temp2->info);
// 	insert_end(86);
// 	insert_end(94);
// 	display();
// 	delete_begin();
// 	display();
// 	return 0;
// }
// static int* buffer;

pthread_mutex_t mutex;

void Producer_Process(void *producer_no)
{
    int object;
    for(int i=0;i<Max;i++)
    {
        // sem_wait(&not_full);
        // if(in==SizeofBuffer) {
        // 	in=0;
        // }
         // pass the srand() parameter  
        object = rand(); 
        pthread_mutex_lock(&mutex);
        // struct node* temp;
        // temp->info = object;
        insert_end(object);
        // display();
        // fp = fopen("Output.txt", "a+");
        printf("Producer %d i: %d Inserted object %d\n",*((int *)producer_no),i,object);
        // fclose(fp);
        // in = in+1;
        // in = in%SizeofBuffer;
        pthread_mutex_unlock(&mutex);
        int val;
        // sem_getvalue(&not_empty,&val);
        // printf("not_empty %d \n",val);
        sem_post(&not_empty); //signals notempty     
        // printf("hua %d",i);
        // sem_getvalue(&not_empty,&val);
        // printf("not_empty %d \n",val);
    }
    // printf("value of not_empty%d \n",not_empty);
    // printf("khatm");
}

void Consumer_Process(void *consumer_no)
{
	// fprintf("inside consumer\n");
    for(int i=0;i<Max;i++)
    {
        sem_wait(&not_empty);
        // if(out==SizeofBuffer)
        // {
        // 	out=0;
        // }
        pthread_mutex_lock(&mutex);
        // int object = buffer[out];
        int object;
        object = delete_begin();
        // display();
        // fp = fopen("Output.txt", "a+");
        printf("Consumer %d i: %d Removed object %d\n",*((int *)consumer_no),i, object);
        // fclose(fp);
        // buffer[out] = -1;
        // out = out+1;
        // out = out%SizeofBuffer;
        pthread_mutex_unlock(&mutex);
        // sem_post(&not_full); //signals not_full
    }
}

int main()
{
	int n=10;
    pthread_t producers[n];
    pthread_t consumers[n];
    pthread_mutex_init(&mutex,NULL);
//     FILE *fp;
        // int myInt = 5;
        // fp = fopen("Output.txt", "a+");// "a+" means that we are going to write on this file
        // fprintf(fp, "This is being written in the file. This is an int variable: %d", myInt);
        
    // sem_init(&not_full, 0, SizeofBuffer);
    sem_init(&not_empty, 0);
    // printf("not_empty %d \n",not_empty);
    // buffer = (int*)malloc(SizeofBuffer * sizeof(int));

    int nos[n];
    for(int i=0;i<n;i++) nos[i]=i+1;
	// pthread_create(&producers[0], NULL, (void *)&Producer_Process, (void *)&nos[0]);
	// pthread_create(&producers[1], NULL, (void *)&Producer_Process, (void *)&nos[1]);
	// pthread_create(&producers[2], NULL, (void *)&Producer_Process, (void *)&nos[2]);
	// pthread_create(&producers[3], NULL, (void *)&Producer_Process, (void *)&nos[3]);
	// pthread_create(&producers[4], NULL, (void *)&Producer_Process, (void *)&nos[4]);
	// pthread_create(&producers[5], NULL, (void *)&Producer_Process, (void *)&nos[5]);
	// // pthread_create(&consumers[0], NULL, (void *)&Consumer_Process, (void *)&nos[0]);
	// pthread_create(&consumers[1], NULL, (void *)&Consumer_Process, (void *)&nos[1]);
	// pthread_create(&consumers[2], NULL, (void *)&Consumer_Process, (void *)&nos[2]);
	// pthread_create(&consumers[3], NULL, (void *)&Consumer_Process, (void *)&nos[3]);
	// pthread_create(&consumers[4], NULL, (void *)&Consumer_Process, (void *)&nos[4]);
	// pthread_create(&consumers[5], NULL, (void *)&Consumer_Process, (void *)&nos[5]);
	
	// pthread_join(producers[0], NULL);
	// pthread_join(producers[1], NULL);
	// pthread_join(producers[2], NULL);
	// pthread_join(producers[3], NULL);
	// pthread_join(producers[4], NULL);
	// pthread_join(producers[5], NULL);
	// // pthread_join(consumers[0], NULL);
	// pthread_join(consumers[1], NULL);
	// pthread_join(consumers[2], NULL);
	// pthread_join(consumers[3], NULL);
	// pthread_join(consumers[4], NULL);
	// pthread_join(consumers[5], NULL);
        time_t t1; // declare time variable   
      
        /* define the random number generator */  
        srand ( (unsigned) time (&t1));
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
    pthread_mutex_destroy(&mutex);
    sem_destroy(&not_empty);
    sem_destroy(&not_full);
    return 0;
}