#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>


void *temporizador(void *args){

    while(1){
        for(int i = 0; i < 5; i++){
            printf("%d ", i);
        }
        printf("\n\n");
        sleep(5);
    }
}

int main(){
    pthread_t thread;
    int tempo = 10;
    pthread_create(&thread, NULL, temporizador, &tempo);
    pthread_join(thread, NULL);
    while(1){
        printf("Esketit");
        sleep(1);
    }
}