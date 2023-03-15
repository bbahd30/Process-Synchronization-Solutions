# Problem Statement

The problem states that there is a bridge which is aligned along the left-right direction. This bridge is too narrow to allow cars to go in both directions. Hence, cars must alternate going across the bridge. The bridge is also not strong enough to hold more than three cars at a time. A solution is to be provided which does not cause starvation. That is, cars that want to get across should eventually get across. However, we want to maximize use of the bridge. Cars should travel across to the maximum capacity of the bridge (that is, three cars should go at one time). If a car leaves the bridge going left and there are no cars going right, then the next left going car should be allowed to cross.

# Solution Requirements

We need a solution that can synchronize the process of the crossing of the given bridge by the upcoming cars from both ends and also simultaneously satisfy the following conditions:

- Mutual Exclusion
    
    As the bridge is narrow, it is being crossed in one direction by the cars which is maximum 3 at a time. Thus, the bridge is the critical section and has to be shared among the cars, which is maximum 3.
    
- Progress
    
    The solution must ensure that any car that is waiting to cross the bridge and has no competition for access should be granted access as soon as possible.
    
- Bounded Waiting
    
    Any car that is waiting to cross the bridge should eventually be granted access, even if there are other cars waiting to cross to ensure that no car is waiting indefinitely and all get equal chances.
    

# StarveFree Solution

- In our [StarveFree Solution](https://github.com/bbahd30/Process-Synchronization-Solutions/blob/master/BridgeCrossing/solution.c), we solved the problem using three semaphores.
- We use the threads to show cars that execute the functions depending on whether they are moving toward the left or right, which are being generated randomly.
- We use an array `carsToCross` that stores a number of cars left to cross from right at first position and from left in the second position.
- In this StarveFree solution,
    - The thread initialized with `rightCar` function acquires the `emutex` semaphore to have mutual exclusion between the cars arriving.
    - After showing its arrival, it increments the its value in the array `carsToCross[1]`, the index is decided based on the direction of the car.
    - The condition of crossing the bridge is checked by looking that the cars on the bridge is less than 3 and the current direction in which cars are moving is same as of the current car.
        - To prevent starvation, we have added one more condition that the cars are allowed to enter, if on the other side, there are no waiting cars.
            - This then doesn’t allow to aquire the `rightSem` or `leftSem` semaphore depending on its direction which makes to stop from crossing the bridge, and waits for signal of the other direction.
        - After it is allowed to cross the bridge it stimulates it using a function and then leaves.
            - While leaving it has to update the shared variables thus, to aquires the `emutex` semaphore, and decrements the value of `carsOnBridge`, then it checks 2 conditions:
                - If the car is the last to leave, i.e., the `carsOnBridge` turns 0, it checks if the car on the other side are waiting or not, if they are then it signals the other side cars to leave, to prevent starvation, which is maximum up to 3 or the number of cars waiting there.
                - If no cars are waiting on other side, then it checks if the cars are waiting on the same side and are signaled to leave, if no cars on waiting on either side, it releases the `emutex` semaphore,
            - This helps to prevent starvation of either side with same priority as they allow the cars on other side to move by waiting for them.
            
            ```cpp
            if ((carsOnBridge == 0) && carsToCross[0] > 0)
                {
                    int carWaiting = min(3, carsToCross[0]);
                    currentDirection = 0;
                    while (carWaiting > 0)
                    {
                        sem_post(&leftSem);
                        carsOnBridge++;
                        carsToCross[0]--;
                        carWaiting--;
                    }
                }
                else if (carsToCross[0] == 0 && carsToCross[1] > 0)
                {
                    sem_post(&rightSem);
                    carsOnBridge++;
                    carsToCross[1]--;
                }
            ```
            

The implementation of right car is as follows:

```cpp
void *rightCar(void *arg)
{
    int carId = *(int *)arg;
    sem_wait(&emutex);
    bridgeArrived(carId, 1);
    carsToCross[1]++;

    if ((carsToCross[0] == 0) && (carsOnBridge < 3 && (currentDirection == 1 || currentDirection == 0)))
    {
        carsOnBridge++;
        currentDirection = 1;
        carsToCross[1]--;
        sem_post(&rightSem); // allowing to access bridge
    }
    sem_post(&emutex);

    sem_wait(&rightSem); // will aquire only if access to bridge allowed by satisfying the conditon

    crossingBridge(carId, 1);
    sleep(2);

    // sem_post(&emutex);
    sem_wait(&emutex);
    leavingBridge(carId, 1);
    carsOnBridge--;

    if ((carsOnBridge == 0) && carsToCross[0] > 0)
    {
        int carWaiting = min(3, carsToCross[0]);
        currentDirection = 0;
        while (carWaiting > 0)
        {
            sem_post(&leftSem);
            carsOnBridge++;
            carsToCross[0]--;
            carWaiting--;
        }
    }
    else if (carsToCross[0] == 0 && carsToCross[1] > 0)
    {
        sem_post(&rightSem);
        carsOnBridge++;
        carsToCross[1]--;
    }
    sem_post(&emutex);
    pthread_exit(NULL);
}
```

Similar for the `leftCar`.

Hence, this handles the synchronisation among the cars on the single-lane bridge allowing 3 at a time to go.