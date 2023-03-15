# The Sushi Bar Problem

<br/>

## Problem Statement

This is a process synchronization problem in which there is a dining table which can accomodate 5 individuals at a time. Whenever a new custumer comes in, they are accomodated at the table until the table is full. Now, if a new customer comes, they cannot be seated at the table and are asked to wait.

However, when a customer leaves, and there is vacant space available at the table, a new customer cannot be accomodated until the whole table leaves, i.e. there is a new crowd at the table. Thus, a customer coming after the table has been occupied to its full capacity can be seated at the table only once the whole table becomes vacant again. 

A simple solution to this can be implemented using three binary semaphores, an integer variable and a boolean variable.
These are as follows:

1) mutex1 : A binary Semaphore to ensure mutual exclusive access of a customer(process) to the Critical Region while entering.
It is initialized to 1.

2) mutex2 : A binary Semaphore to ensure mutual exclusive access of a customer(process) to the Critical Region while leaving.
It is initialized to 1.

3) block : A binary Semaphore to ensure that no new customer(process) can be seated at the table once the table becomes full.
It is initialized to 0.

4) eating : An integer variable acting as a counter to count the number of customers seated at the table.
It is initialized to 0.

5) waiting : An integer variable acting as a counter to count the number of customers waitng to sit at the table.
It is initialized to 0.

6) must_wait: A boolean variable showing whether the incoming customer must wait or not.

<br/>

## Pseudocode

The pseudocode for the solution is as follows:

```
// Values initialised to
// mutex1 = 1
// block = 0
// mutex2 = 1
// eating = 0
// waitng = 0


mutex1.wait() 

if must_wait:
  waiting += 1 
  mutex1.signal() 
  block.wait()
  //thread blocked
  
else:
  eating += 1
  must_wait = (eating == 5)
  //updating the value of boolean
  mutex1.signal()
  
  
// Time to eat sushi


mutex2.wait() 
eating -= 1
if eating == 0:
  n = min(5, waiting)
  // n is an int denoting the number of customers in the next round
  waiting -= n
  eating += n
  
  must_wait = (eating == 5)
  //updating the boolean variable
  block.signal(n)
  //signalling a thread to wake up
  
mutex2.signal()

```


