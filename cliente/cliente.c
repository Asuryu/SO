// ISEC - Trabalho Prático de SO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "cliente.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>

void handler_funcSignal(int signum){ // Para o sa_handler com (completo - PIDs, etc...)
    printf("\nEstou vivo meu puto!\n");
}

int main(int argc, char *argv[]){

    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n\n");
    char *mso_state = getenv("MEDICALSO_RUN");
    if(mso_state == NULL){
        printf("[CLIENTE]\nO balcão está fora de serviço\n");
        return 0;
    }
    if(argv[1] == NULL){
        printf("[CLIENTE]\nPor favor insira um nome\nUtilização: ./cliente [nome]\n");
        return 0;
    }
    else printf("[CLIENTE]\nBem vindo ao MEDICALso, %s\n", argv[1]);

    struct sigaction sa;
    sa.sa_handler = handler_funcSignal;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGALRM, &sa, NULL);
    // Criar alarme com o signal
    while(1){
        alarm(1);
        pause();
    }

    return 0;
}