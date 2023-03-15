#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "../Semaphore.h"

#define Smokers 3      //Number Of smokers, you can change the number of smokers

int SmokerT=0;
int SmokerM=0;
int SmokerP=0;
int S=Smokers;

struct Semaphore semT,semP,semM;    //Defining Semaphores for Smokers
struct Semaphore semAgent;          //Defining Semaphores for Agent
     
int Match = 0;          //Defining Items for smoking
int Paper = 0;
int Tabacco = 0;


/*



Considering One of the smokers have infinite Supply
for one of the items.
In This code 
Smoker 1 have infinite Supply for Tabacco
Smoker 2 have infinite Supply for Paper
Smoker 3 have infinite Supply for Matches



*/

/*--------------Smoker Section-------------------*/
void *Smoker(void *arg){   
    int id=*(int*)arg;

    while(1){

        switch (id)
        {
        case 0:
            sem_wait(&semT);
            if(Match>0 && Paper>0)          //Checking if Items are Availble for Smoker 1
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
            else 
            printf("%d: Error: Paper or match is not avalible!\n", id+1);
            break;
        case 1:
            sem_wait(&semP);
            if(Match>0 && Tabacco>0)        //Checking if Items are Availble for Smoker 2
            {
                Match=0;
                Tabacco=0;
                printf("Smoker %d: Got paper and match\n", id+1);
                printf("Smoker 2 is smoking\n");
                /* 
            
                CRITICAL SECTION
            
                */
                usleep(1000000);
                SmokerP++;
                sem_post(&semAgent);
            }
            else 
            printf("%d: Error: Tabacco or match is not avalible!\n", id+1);
            break;
        case 2:
            sem_wait(&semM);
            if(Tabacco>0 && Paper>0)        //Checking if Items are Availble for Smoker 3
            {
                Paper=0;
                Tabacco=0;
                printf("Smoker %d: Got paper and match\n", id+1);
                printf("Smoker 3 is smoking\n");
                /* 
            
                CRITICAL SECTION
            
                */
                usleep(1000000);
                SmokerM++;
                sem_post(&semAgent);
            }
            else 
            printf("%d: Error: Paper or Tabacco is not avalible!\n", id+1);
            break;        
        
        default:
        printf("ERROR: Invalid argument ID: %d\n", id);
        exit(1);
            break;
        }
        if(SmokerM > Smokers && SmokerT > Smokers && SmokerP > Smokers)
        exit(1);
    }


}

/*--------------Agent Section-------------------*/
void *Agent(void * arg){

  while (1)
  {
        sem_wait(&semAgent);

        usleep(1000000);

        int id=rand()%3;        //Agent Selecting A random Item

        //Remove this Segment to make it run for infinite times.
        if(SmokerT>S && id==0)         
        id=id+1;
        if(SmokerP>S && id==1)
        id=id+1;
        if(SmokerM>S && id==2)
        id=0;
        
        
       
        switch (id)
        {
        case 0:             //Agent putting Paper and Match
            printf("Agent putting Match and Paper on the table\n");
            Match++;
            Paper++;
            sem_post(&semT);
            break;
        case 1:             //Agent putting Match and Tabacco
            printf("Agent putting Match and Tabacco on the table\n");
            Match++;
            Tabacco++;
            sem_post(&semP);
            break;  
        case 2:             //Agent putting Paper and Tabacco
            printf("Agent putting Paper and Tabacco on the table\n");
            Paper++;
            Tabacco++;
            sem_post(&semM);
            break;      
        
        default:            //Error Case
            printf("Incorrect Agent ID \n");
            break;
        }
    }
}


int main(int argc, char *argv[]){


// sem_init(&semM,0,0);
// sem_init(&semP,0,0);
// sem_init(&semT,0,0);
sem_init(&semM,0);
sem_init(&semP,0);
sem_init(&semT,0);

sem_init(&semAgent,1);    
// sem_init(&semAgent,0,1);    

pthread_t ptSmoker[Smokers],ptAgent;

int smokerid[Smokers];

for(int i=0;i<Smokers;i++){
    smokerid[i]=i%3;      //If number of smokers are greater than 3 than it will make it work for the other cases too
    pthread_create(&ptSmoker[i], NULL , Smoker , &smokerid[i]);
}
puts("Smokers Created");


pthread_create(&ptAgent,NULL,Agent,NULL);   
puts("Agent Created");


for(int i=0;i<Smokers;i++){
    pthread_join(ptSmoker[i],NULL);
}


pthread_join(ptAgent,NULL);



}
