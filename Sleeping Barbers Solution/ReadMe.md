
Classical Sleeping-Barber problem:
This is a very standard problem in process synchronisation, in which there is a hypothetical barber shop,
having one barber, one barber chair, and N chairs for the customers to wait in.
The rules are the following:
-> While there are no customers in the shop, the barber sleeps in his own chair.
-> When a customer arrives, he wakes up the barber.
-> If customer arrives and barber is busy giving another customer a haircut and if any of the
waiting chair is empty, then the customer occupies the chair and waits for the barber, else the
customer leaves the shop.
Solution to the classical problem:
There are several solutions implemented using 'mutex', which will ensure mutual exclusion between
the customers who can get served at once. The following solution is starvation-free and deadlock-free
as shown below:
There are 3 semaphores used:
1) A binary semaphore to indicate whether there is a customer present in the barber's chair or not (barber_sleep).
2) A binary semaphore to indicate whether customers are waiting in the chairs (waiting_cust)
3) A binary semaphore to be acquired by the customers to access the waiting chairs (access_wait_chairs)
The procedure is executed in the way as shown:
When a customer arrives, he first acquires the access_wait_chair semaphore and checks if ther is a free seat available.
If so, he will occupy the free chair, else he will leave the shop. In both the cases, he will release the semaphore.
In case the customer occupies a chair, the waiting_cust semaphore will be increased by one.
Once the customers are inside the waiting chairs, they will be served in the FIFO order. This will ensure no starvation of
any customer in the solution. Next, the customer who will recieve a haircut will acquire the barber_sleep semaphore, decrement the
waiting_cust semaphore, and will release it once he's done getting a haricut.
The psuedocode for the following solution can be shown as follows:

'''c
semaphore barber_awake=0 //initially the barber is sleeping
semaphore access_wait_chairs=1 // if this is 1, then the waiting chairs can be incremented or decremented
semaphore waiting_cust=0 //intially there are no customers in the shop
int FreeSeats=N;
Customer(){
    do{
        Down(access_wait_chairs) // a customer initially acquires the semaphore to access the waiting chairs
        
        if(FreeSeats<0){
            Up(access_wait_chairs)
            *** LEAVE WITHOUT A HAIRCUT ***
            continue
        }
        Up(barber_awake)
        FreeSeats=Freaseats-1 //decrease the number of waiting free seats by 1
        up(waiting_cust) //indicate that there are waiting customers in the chairs
        Up(access_wait_chairs) //release this semaphore so that other customers can enter the shop
        
        Down(barber_awake) //wait for the barber to wake up
        *** GET A HAIRCUT ***
    }while(true)
}
Barber(){
    do{
        Down(waiting_cust) //check whether there are waiting customers in the chairs
        Down(access_wait_chairs) // this is done so that the barber can check for waiting customers 
        FreeSeats=FreeSeats+1
        Up(access_wait_chairs) // release the hold of chairs now
        Up(barber_awake) //indicate that the barber is ready to make haircuts
        *** CUT HAIR ***
    }while(true);
}
'''

Now the given problem is the one that consists of multiple barbers, multiple customers and a single queue.
This can be easily solved without having to use any additional semaphores by using the concept of multithreading.
A thread which executes Barber() has to be initialised for each barber in the shop.
The follwing points must be ensured for the multiple barber problem:
1) Mutiple barbers should not cut the hair of same customer
2) The situation of having only one busy barber while all other barbers are asleep should be avoided
