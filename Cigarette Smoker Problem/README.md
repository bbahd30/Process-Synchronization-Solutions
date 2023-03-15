# CSN-232 Cigrattes Smokers Problem 

## Problem Statement

Cigarette Smokers Problem is a concurrency problem.
The problem is:
Assume a cigarette requires three ingredients to make and smoke by the `Smokers`. The ingredients include- `tabacco, matches and paper`.
There are `3 `smokers around a table, each of them have a `infinite supply`of one of the `three ingredients`, i.e. one smoker has an infinite supply of tabacco, another smoker has matches and the last one has paper. 
There's also a non-smoker `Agent` who gives supply to the smokers and enables them by selecting and giving any two items from the table,using them smokers make a cigarette, which they smoke for a while and they ask for more.Once the smoker finished smoking the agant again puts two random items on the table and this process continues forever.


 - - - -
 
## Pseudocode

### Smokers 

```c
Smoker(){   
            sem_wait(&semT);
            if(Match>0 && Paper>0)        
            {
                Match=0; 
                Paper=0;
                printf("Smoker %d: Got paper and match\n", id+1);
                printf("Smoker 1 is smoking\n");
                /* 
            
                CRITICAL SECTION
            
                */
                usleep(1000000);
                SmokerT++;
                sem_post(&semAgent);
            }
         } 


```

The above example is for Smoker who have infite supply for Tabacco.
Similarly just by changing the variables we will get functions for all the smokers.
Smoker will wait for agent to put the item once it is being there it will select the item and smoke for a while then after finishing will wait for the agent to give more ingredients


### Agent

```c
void *Agent(void * arg){

  while (1)
  {
        sem_wait(&semAgent);

        int id=rand()%3;        //Agent Selecting A random Item

            
        for id=0
            Match++;
            Paper++;
            sem_post(&semT);
            
            
         for id=1
            Match++;
            Tabacco++;
            sem_post(&semP);
          
          for id=2
            Paper++;
            Tabacco++;
            sem_post(&semM);
      
     
       
}


```

The above code is an example for Agent, After Selecting an item randomly the agent the agent puts that on the table and wait till it has been used

 - - - -
 
 
In the code we can change the number of smokers `n` , and the code will run till all smokers smoked n times. By `Removing 2 segments ` mentioned in the code the code will run for `infinite times`.
I let that Smoker have to smoke atlease n times, after that, That particular smoker will not smoke again. 
Thus This is the solution for `Cigarette Smoker Problem Starvation free and Deadlock Free`.
