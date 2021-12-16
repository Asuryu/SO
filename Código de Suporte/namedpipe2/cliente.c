#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#define SERVER_FIFO "SERVIDOR"
#define CLIENT_FIFO "CLIENTE[%d]"

char CLIENTE_FIFO_FINAL [100];

typedef struct 
{
    pid_t pid;
    char operador;
    int num1;
    int num2;
}dataMSG;

typedef struct
{
    float res;
}dataRPL;

int main()
{
    dataMSG operacao;
    dataRPL resposta;
    operacao.pid = getpid();
    int fd_envio, fd_resposta;

    sprintf(CLIENTE_FIFO_FINAL, CLIENT_FIFO, getpid());
    if(mkfifo(CLIENTE_FIFO_FINAL,0666) == -1){
        if(errno == EEXIST){
            printf("FIFO ja existe!\n");
        }
        printf("Erro ao abrir fifo!\n");
        return 1;
    }

    do{
        printf("Cliente [%5d]\n", operacao.pid);
        printf("Introduza o operador:");
        fflush(stdin);
        printf("\n");
        scanf(" %c", &operacao.operador);
        if(operacao.operador != '.')
        {
            printf("Introduza o numero 1:");
            scanf("%d", &operacao.num1);
            printf("\n");
            printf("Introduza o numero 2:");
            scanf("%d", &operacao.num2);
            printf("\n");
        }
        else
        {
            fd_envio = open(SERVER_FIFO, O_WRONLY);
            int size = write(fd_envio, &operacao, sizeof(operacao));
            close(fd_envio);
            unlink(CLIENTE_FIFO_FINAL);
            exit(1);
        }
        fd_envio = open(SERVER_FIFO, O_WRONLY);
        int size = write(fd_envio, &operacao, sizeof(operacao));
        close(fd_envio);
        fd_resposta = open(CLIENTE_FIFO_FINAL, O_RDONLY);
        int size2 = read(fd_resposta, &resposta, sizeof(resposta));
        if(size2 > 0){
            close(fd_resposta);
            printf("\nResultado do calculo: [%f]\n", resposta.res);
        }
    }while(1);

    unlink(CLIENTE_FIFO_FINAL);
}