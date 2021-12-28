// ISEC - Trabalho Prático de SO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "cliente.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BALCAO_FIFO "../MEDICALso"
#define BALCAO_FIFO_CLI "../MEDICALsoCLI"
#define CLIENTE_FIFO "../CLIENTE[%d]"

char CLIENTE_FIFO_FINAL[MAX];

void handler_funcSignal(int signum){ // Para o sa_handler com (completo - PIDs, etc...)
    printf("\nEstou vivo meu puto!\n");
}

int balcaoAberto(){
    int fd_balcao = open(BALCAO_FIFO, O_RDONLY | O_NONBLOCK);
    close(fd_balcao);
    if(fd_balcao == -1){
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]){

    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n\n");
    if(!balcaoAberto()){
        printf("[CLIENTE]\nO balcão está fora de serviço\n");
        return 0;
    }
    if(argv[1] == NULL){
        printf("[CLIENTE]\nPor favor insira um nome\nUtilização: ./cliente [nome]\n");
        return 0;
    }
    else printf("[CLIENTE]\nBem vindo ao MEDICALso, %s\n", argv[1]);

    cliente c;

    sprintf(CLIENTE_FIFO_FINAL, CLIENTE_FIFO, getpid());
    if(mkfifo(CLIENTE_FIFO_FINAL,0666) == -1){
        if(errno == EEXIST){
            printf("FIFO ja existe!\n");
        }
        printf("Erro ao abrir fifo!\n");
        return 1;
    }

    c.pid = getpid();
    strcpy(c.nome, argv[1]);
    int fd_envio = open(BALCAO_FIFO_CLI, O_WRONLY | O_NONBLOCK);
    int size = write(fd_envio, &c, sizeof(cliente));

    char resposta[MAX];
    int fd_recebe = open(CLIENTE_FIFO_FINAL, O_RDONLY);
    int size = read(fd_recebe, resposta, sizeof(resposta));
    if(!strcmp("ERROR 400 - LIMITE ATINGIDO", resposta))
        printf("\nLimite de Médicos atingido\n");
    else if(!strcmp("SUCCESS 200 - ACEITE", resposta))
        printf("\nMédico aceite!\n");
    close(fd_recebe);
    close(fd_envio);
    unlink(CLIENTE_FIFO_FINAL);

    // struct sigaction sa;
    // sa.sa_handler = handler_funcSignal;
    // sa.sa_flags = SA_RESTART | SA_SIGINFO;
    // sigaction(SIGALRM, &sa, NULL);
    // // Criar alarme com o signal
    // while(1){
    //     alarm(1);
    //     pause();
    // }

    return 0;
}