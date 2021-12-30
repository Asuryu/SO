#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
  
// A normal C function that is executed as a thread 
// when its name is specified in pthread_create()
void *myThreadFun(void *vargp)
{
    while(1){
        sleep(5);
        printf("Printing GeeksQuiz from Thread \n");
    }
}

void *myThreadFun2(void *vargp)
{
    int i = 0;
    while(1){
        sleep(1);
        printf("%d\n", i++);
    }
}

int main()
{
    pthread_t thread_id;
    printf("Before Thread\n");
    pthread_create(&thread_id, NULL, myThreadFun, NULL);
    //pthread_create(&thread_id, NULL, myThreadFun2, NULL);
    pthread_join(thread_id, NULL);
    while(1){
        printf("Esketit");
        sleep(1);
    }
    printf("After Thread\n");
    exit(0);
}