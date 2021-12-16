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
#define CLIENTE_FIFO "CLIENTE[%d]"

char CLIENT_FIFO_FINAL[100];

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

void handler_sigalrm(int s, siginfo_t *i, void *v)
{
    unlink(SERVER_FIFO);
    printf("\nAdeus\n");
    exit(1);
}

int main()
{
    struct sigaction sa;
        sa.sa_sigaction = handler_sigalrm;
        sa.sa_flags = SA_RESTART|SA_SIGINFO;
    sigaction(SIGINT,&sa,NULL);
    dataMSG operacao; // rename basically
    dataRPL resposta;

    if(mkfifo(SERVER_FIFO,0666) == -1){
        if(errno == EEXIST){
            printf("FIFO ja existe!\n");
        }
        printf("Erro ao abrir fifo!\n");
        return 1;
    }

    //mkfifo(SERVER_FIFO,0666); //só um é que abre
    int fdRecebe = open(SERVER_FIFO,O_RDONLY);
    if(fdRecebe == -1){printf("Erro\n"); return 1;}
    do{
        int size = read(fdRecebe, &operacao,sizeof(operacao));
        if(size > 0){
        //realiza operacao
        //enviar para o cliente[PID] certo
        int resultado;
        printf("Ligado ao cliente [%5d]\n", operacao.pid);
        if(operacao.operador == '+')
        {
            resultado = operacao.num1 + operacao.num2;
            printf("Cliente [%5d] -> Operacao: [%d %c %d]; Resultado: [%d]\n",operacao.pid, operacao.num1, operacao.operador, operacao.num2, resultado);
        }
        else if(operacao.operador == '-')
        {
            resultado = operacao.num1 - operacao.num2;
            printf("Cliente [%5d] -> Operacao: [%d %c %d]; Resultado: [%d]\n",operacao.pid, operacao.num1, operacao.operador, operacao.num2, resultado);
        }
        else if(operacao.operador == '/')
        {
            resultado = operacao.num1 / operacao.num2;
            printf("Cliente [%5d] -> Operacao: [%d %c %d]; Resultado: [%d]\n",operacao.pid, operacao.num1, operacao.operador, operacao.num2, resultado);
        }
        else if(operacao.operador == 'x' || operacao.operador == '*')
        {
            resultado = operacao.num1 * operacao.num2;
            printf("Cliente [%5d] -> Operacao: [%d %c %d]; Resultado: [%d]\n",operacao.pid, operacao.num1, operacao.operador, operacao.num2, resultado);
        }
        else if(operacao.operador == '.')
        {
            kill(getpid(), SIGINT);
        }

        resposta.res = resultado;

        sprintf(CLIENT_FIFO_FINAL, CLIENTE_FIFO, operacao.pid);
        int fdResponde = open (CLIENT_FIFO_FINAL, O_WRONLY);
        if(fdResponde == -1){printf("Erro\n");}
        int size2 = write(fdResponde, &resposta, sizeof(resposta));
        close(fdResponde);
        }
    }while(1);

    close(fdRecebe);
    unlink(SERVER_FIFO);
}