# The Sushi Bar Problem

<br/>

## Problem Statement

This is a process synchronization problem in which there is a dining table which can accomodate 5 individuals at a time. Whenever a new custumer comes in, they are accomodated at the table until the table is full. Now, if a new customer comes, they cannot be accomodated at the table and are asked to wait.

However, when a customer leaves, and there is vacant space available at the table, a new customer cannot be accomodated until the whole table leaves, ie there is a new crowd at the table. Thus, a customer coming after the table has been occupied to its full capacity once can be seated at the table only once the whole table becomes vacant again. 


A simple solution to this can be implemented using two binary semaphores, a counting semaphore and an integer variable.
These are as follows:

1) mutex : A binary Semaphore to ensure mutual exclusive access of a customer(process) to the Critical Region while entering or leaving.
It is initialized to 1.

2) lock : A binary Semaphore to ensure that no new customer(process) can be seated at the table until the table(executing in the CR) is starting from fully vacant state.
It is initialized to 1.

3) seats : A counting Semaphore to check that only upto 5 customers(processes) are seated at the table(executing in the CR) at a time.
It is initialized to 5 to represent 5 vacant seats.

4) customers : An integer variable acting as a counter to count the number of customers seated at the table.
It is initialized to 0.

<br/>

## Pseudocode

The pseudocode for the solution is as follows:

```
// Values initialised to
// mutex = 1
// lock = 1
// seats = 5
// customers = 0
```


<br/>
Function for Seating a customer at the table - 

```
enter_customer()
{

    wait(lock);
    wait(seats);
    signal(lock);

    wait(mutex);
    customers ++;

    if(customers == 5)
    {
        wait(lock);
    }


    ***********
    Executes in Critical Region
    ***********


    signal(mutex);
}
```


<br/>
Function for a customer to exit -

```
exit_customer()
{
    wait(mutex);

    customers --;

    if(customers == 0)
    {
        signal(lock);
    }


    signal(mutex);
    signal(seats);
}
```


