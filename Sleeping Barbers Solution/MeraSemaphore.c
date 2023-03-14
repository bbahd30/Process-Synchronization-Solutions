#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<stdatomic.h>

struct Merasemaphore{
    volatile atomic_int value;
    volatile atomic_flag mutex;

}/*Declare Semaphore Variables here*/;

void sem_wait(struct Merasemaphore *s){
    while(atomic_flag_test_and_set(&s->mutex));
    while(atomic_load(&s->value)<=0);
    atomic_fetch_sub(&s->value, 1);
    atomic_flag_clear(&s->mutex);
}

void sem_init(struct Merasemaphore *s, int value){
      atomic_init(&s->value,value);
}

void sem_post(struct Merasemaphore *s){
        atomic_fetch_add(&s->value,1);
}

void sem_destroy(struct Merasemaphore *s){
    while(&s->value>0)sem_post(s);
}
