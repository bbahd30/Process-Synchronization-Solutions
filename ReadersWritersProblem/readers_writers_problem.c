#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "../Semaphore.h"

struct Semaphore read_mutex,order,access_mutex;

int shared_data=10; // data to be shared by readers and writers

int readerscount=0; // number of readers reading 

void *reader_function(void*rid){
    usleep(10);
    int i=*((int *)rid);
    sem_wait(&order); // wait if any readers are reading or writers are writing
    sem_wait(&read_mutex); // readerscount modification

    readerscount++;

    if(readerscount==1){
        sem_wait(&access_mutex); // makes writer process wait for critical section if atleast 1 reader is reading already
    }

    sem_post(&read_mutex); //readerscount modification finished
    sem_post(&order); //frees process for writer to enter

    /*critical section*/
    printf("Reader %i is reading data %i \n",i,shared_data); 

    sem_wait(&read_mutex); //readerscount modification
    readerscount--;
    sem_post(&read_mutex); //readerscount modification finished
    if(readerscount==0){
        sem_post(&access_mutex); //if no readers are reading then gives acces of critical section to writer process
    }
    
    return 0;
}

void *writer_function(void *wid){
    
    usleep(10);
    int i=*((int *)wid);
    
    sem_wait(&order); // wait if any readers are reading or writers are writing
    sem_wait(&access_mutex); // wait for critical section if any other process is on critical section
    
    sem_post(&order); //frees entry of any other process
    
    /*critical section*/

    shared_data+=10;
    printf("writer %i is writing data= %i \n",i,shared_data); 


    sem_post(&access_mutex); //frees access to critical section
    
    return 0;
}

int main(){
    //initializing all three semaphores

    sem_init(&read_mutex,1);
    sem_init(&access_mutex,1);
    sem_init(&order,1);
    
    int num_readers=20,num_writers=20,maxid;
    maxid=(num_readers>=num_writers)?num_readers:num_writers;
    int id[maxid];

    pthread_t readers[num_readers],writers[num_writers];
    
    for(int i=0;i<maxid;i++){
        id[i]=i+1;
    }

    for(int i=0;i<num_readers;i++){
        pthread_create(&readers[i],NULL,reader_function,(void *)&id[i]);
        pthread_create(&writers[i],NULL,writer_function,(void *)&id[i]);
    }
    
    for(int i = 0; i < num_readers; i++)
    {
        pthread_join(readers[i], NULL); 
        pthread_join(writers[i], NULL);
    }

    // for(int i=0;i<num_writers;i++){
    //     pthread_create(&writers[i],NULL,writer_function,(void *)&id[i]);
    // }

    // for(int i = 0; i < num_writers; i++)
    // {
    //     pthread_join(writers[i], NULL);
    // }

    //destroying all sempahores
    sem_destroy(&order);
    sem_destroy(&access_mutex);
    sem_destroy(&read_mutex);

    return 0;
}