# Problem Statement
The problem states that there is a bridge which is aligned along the left-right direction. This bridge is too narrow to allow cars to go in both directions. Hence, cars must alternate going across the bridge. The bridge is also not strong enough to hold more than three cars at a time. A solution is to be provided which does not cause starvation. That is, cars that want to get across should eventually get across. However, we want to maximize use of the bridge. Cars should travel across to the maximum capacity of the bridge (that is, three cars should go at one time). If a car leaves the bridge going left and there are no cars  going right, then the next left going car should be allowed to cross. 


# Solution Requirements

We need a solution that can synchronize the process of the crossing of the given bridge by the upcoming cars from both ends and also simultaneously satisfy the following conditions:

- Mutual Exclusion
    
    As the bridge is narrow, it is being crossed in one direction by the cars which is maximum 3 at a time. Thus, the bridge is the critical section and has to be shared among the cars, which is maximum 3.
    
- Progress
    
    The solution must ensure that any car that is waiting to cross the bridge and has no competition for access should be granted access as soon as possible.
    
- Bounded Waiting
    
    Any car that is waiting to cross the bridge should eventually be granted access, even if there are other cars waiting to cross to ensure that no car is waiting indefinitely and all get equal chances


# Initial Approach

- In our [initial approach](https://github.com/bbahd30/Process-Synchronization-Solutions/blob/master/BridgeCrossingProblem/bridgeCrossingSol.cpp), we solved the problem using a semaphore
    - `mutex`
        - This lock, initialized with 1, ensures that there is no race condition in the reading and updating of the variables that control access to the critical section.
- We use the threads to show cars that execute the functions depending on whether they are moving toward the left or right.
- In this solution,
    - We first lock `mutex` to allow access only to a single thread at a time to read variables.
        - `currentDirection` is 0 if no direction has been set, else `1` for right and `-1` for left.
        - `carsOnBridge` shows the number of cars currently running on the bridge.
    - The `while` loop runs till the threads find the condition of entering the bridge satisfied.
        - The condition states that if the current direction is not set or is the same as of the car and the current number of cars is less than the maximum capacity or if the direction is opposite but has no cars on the other end, then the current car is allowed to enter the bridge.
            - After which it sets `currentDirection` and increments `carsOnBridge`.
            - And unlocking the `mutex`.
            - And then enters the critical section `bridge`.
        - If not, then the car waits and releases the `mutex`.
- The function to execute for the right-moving cars is as follows:
    
    ```cpp
    void *rightCar(void *arg)
    {
        pthread_mutex_lock(&mutex);
        int carId = *(int *)arg;
        bridgeArrived(carId, 1);
        while (true)
        {
            if (((currentDirection == 1 || currentDirection == 0) && carsOnBridge < 3) || ((currentDirection == -1) && carsOnBridge == 0))
            {
                currentDirection = 1;
                carsOnBridge++;
                crossingBridge(carId, 1);
                pthread_mutex_unlock(&mutex);
    
                sem_wait(&bridgeSem);
                sleep(1);
                sem_post(&bridgeSem);
    
                pthread_mutex_lock(&mutex);
                carsOnBridge--;
                leavingBridge(carId, 1);
                pthread_mutex_unlock(&mutex);
                break;
            }
            else
            {
                waitingCar(carId, 1);
                pthread_mutex_unlock(&mutex);
                sleep(1);
            }
        }
        pthread_exit(NULL);
    }
    ```
    
    Similar for the `leftCar`.
    

## Analysis

- The above-stated solution satisfies the 3 parameters stated at the beginning to the following extent:
    - Mutual Exclusion
        - `mutex` lock allows the single thread exclusion.
        - `bridgeSem` semaphore allows a maximum of 3 threads to share the bridge as needed.
    - Progress
        - Any car that arrives gets a chance to cross the bridge, hence providing progress.
        - `currentDirection` and semaphores ensure that there is no deadlock.
    - Bounded Waiting
        - Any car that reaches when the `currentDirection` is the same as its direction then the car is allowed to go when the `bridgeSem` is released.
        - However, the arrived car which has the opposite direction has to wait until the direction changes, i.e., till `carsOnBridge` turns 0.
            - Though this ensures that the cars on either side have bounded waiting and aren’t waiting indefinitely to cross the bridge.
            - The arrival of the cars moving in the same direction, one after the other, may lead to starvation of the car to move in the opposite direction.
- Moreover, `while` loop we are using also makes the program inefficient.


# StarveFree Solution

- In our [StarveFree Solution](https://github.com/bbahd30/Process-Synchronization-Solutions/blob/master/BridgeCrossingProblem/starveFreeBridgeSoln.cpp), we solved the problem using two semaphores as we did in initial aproach
- We use the threads to show cars that execute the functions depending on whether they are moving toward the left or right.
- We also use an array 'carsToCross' that stores number of cars to be crossed from right at first position and from left in second position.
- In this StarveFree solution,
    - We first lock `mutex` to allow access only to a single thread at a time to read variables.
        - `currentDirection` is 0 if no direction has been set, else `1` for right and `-1` for left.
        - `carsOnBridge` shows the number of cars currently running on the bridge.
    - The `while` loop runs till the threads find the condition of entering the bridge satisfied.
        - Here condition states that if the current direction is not set or is the same as of the car and the current number of cars is less than the maximum capacity, then the current car is allowed to enter the bridge.
            - After which it sets `currentDirection` and increments `carsOnBridge`.
            - Decrements the first element of 'carsToCross'.
            - And unlocking the `mutex`.
            - And then enters the critical section `bridge`.
        - After the car crosses the bridge, other side of the bridge is checked to check if it was the last one on the bridge to prevent 'Starvation' 
        - It is checked if it was the last one on bridge, then if it was last we need to check both the sides and look the number of cars on the other side waiting, if there is no car on left, then right-moving cars move.
        - If not, then the car waits and releases the `mutex`.
- The function to execute for the right-moving cars is as follows:
    
    ```cpp
    void *rightCar(void *arg)
   {
    int carId = *(int *)arg;

    pthread_mutex_lock(&mutex);
    bridgeArrived(carId, 1);
    while (true)
    {
        if (((currentDirection == 1 || currentDirection == 0) && carsOnBridge < 3))
        {
            currentDirection = 1;
            carsOnBridge++;
            carsToCross[1]--;
            crossingBridge(carId, 1);
            pthread_mutex_unlock(&mutex);

            sem_wait(&bridgeSem);
            sleep(2);
            sem_post(&bridgeSem);

            pthread_mutex_lock(&mutex);
            carsOnBridge--;
            leavingBridge(carId, 1);
            // after crossing the bridge need to take care if the last one on bridge, then have to check both the sides
            // to prevent starvation, should look the number of cars on the other side waiting, if no one of left, then right-moving cars move
            if (carsOnBridge == 0 && carsToCross[0] > 0)
                currentDirection = -1;
            pthread_mutex_unlock(&mutex);
            break;
        }
        else
        {
            waitingCar(carId, 1);
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }
    pthread_exit(NULL);
   }
    ```
    
    Similar for the `leftCar`.
    
