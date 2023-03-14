#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<stdatomic.h>

struct Merasemaphore{
    volatile atomic_int value;      //Value of the semaphore
    volatile atomic_flag mutex;     //Flag to ensure mutual exclusion, it is lock free atomic_bool tye variable

}/*Declare Semaphore Variables here*/;

void sem_wait(struct Merasemaphore *s){ 
    while(atomic_flag_test_and_set(&s->mutex));     //sets the variable mutex to true atomically 
    while(atomic_load(&s->value)<=0);               //wait until the value of the semaphore is positive
    atomic_fetch_sub(&s->value, 1);                 //subtract one from semaphore
    atomic_flag_clear(&s->mutex);                   //release mutex, and set the variable to false atomically
}

void sem_init(struct Merasemaphore *s, int value){
      atomic_init(&s->value,value);         //initialises the value of the semaphore non-atomically
}

void sem_post(struct Merasemaphore *s){
        atomic_fetch_add(&s->value,1);      //increments the value of the semaphore by 1 (which is non-atomic) atomically
}

void sem_destroy(struct Merasemaphore *s){
    while(!(s->value))sem_post(s);         //this function destroys the semaphore which was previously initiated with the function sem_init()
}