#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

struct Merasemaphore{
    volatile _Atomic int value;
    volatile atomic_flag mutex;

}read_mutex,access_mutex,order; 
//order= sempahore to control the entry and exit of readers and writers processes 
//access_mutex= sempahore to ensure mutual exclusion in critical section
//read_mutex= semaphore to update readerscount
void wait(struct Merasemaphore *s){
    while(atomic_flag_test_and_set(&s->mutex));
    while(atomic_load(&s->value)<=0);
    atomic_fetch_sub(&s->value, 1);
    atomic_flag_clear(&s->mutex);
}

void sem_init(struct Merasemaphore *s, int value){
      atomic_init(&s->value,value);
}

void signal(struct Merasemaphore *s){
        atomic_fetch_add(&s->value,1);
}

void sem_destroy(struct Merasemaphore *s){
    while(!(s->value))signal(s);
}

int shared_data=10; // data to be shared by readers and writers

int readerscount=0; // number of readers reading 

void *reader_function(void*rid){
    usleep(10);
    int i=*((int *)rid);
    wait(&order); // wait if any readers are reading or writers are writing
    wait(&read_mutex); // readerscount modification

    readerscount++;

    if(readerscount==1){
        wait(&access_mutex); // makes writer process wait for critical section if atleast 1 reader is reading already
    }

    signal(&read_mutex); //readerscount modification finished
    signal(&order); //frees process for writer to enter

    /*critical section*/
    printf("Reader %i is reading data %i \n",i,shared_data); 

    wait(&read_mutex); //readerscount modification
    readerscount--;
    signal(&read_mutex); //readerscount modification finished
    if(readerscount==0){
        signal(&access_mutex); //if no readers are reading then gives acces of critical section to writer process
    }
    
    return 0;
}

void *writer_function(void *wid){
    
    usleep(10);
    int i=*((int *)wid);
    
    wait(&order); // wait if any readers are reading or writers are writing
    wait(&access_mutex); // wait for critical section if any other process is on critical section
    
    signal(&order); //frees entry of any other process
    
    /*critical section*/

    shared_data+=10;
    printf("writer %i is writing data= %i \n",i,shared_data); 


    signal(&access_mutex); //frees access to critical section
    
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