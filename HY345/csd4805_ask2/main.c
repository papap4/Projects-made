#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>


#define THREAD_NUMS 4
sem_t *mutex;
pthread_barrier_t barrier;

void *student(void *param)
{
    int time;
    int i = (int *) param;
    time = getRand();
    
    printf("student %d begins studying for %d seconds\n", i, time);
    sleep(time);
    printf("----------------student %d done studying after %d seconds\n", i, time);
  
    pthread_barrier_wait(&barrier);    
}


int getRand(){
    srand(pthread_self());
    return (rand() % (15-5 +1)) +5;
}

int main(void)
{
    int i=0;
    pthread_t t[9];
    sem_init(&mutex, 0, 3);

    pthread_barrier_init(&barrier, NULL, THREAD_NUMS);
    for ( i = 0; i < 9; i++)
    {
        
        
        pthread_create(&t[i], NULL, student, (void *) i+1);
        if (i == 2 || i == 5 || i==8)
        {
            
            pthread_barrier_wait(&barrier);
            pthread_barrier_destroy(&barrier);
            pthread_barrier_init(&barrier, NULL, THREAD_NUMS);
            printf("______________________________\n");
        }
    }
    
    
    

    

    printf("all sub threads ready, go!\n");

   

}