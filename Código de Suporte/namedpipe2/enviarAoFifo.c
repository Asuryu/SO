#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define SERVER_FIFO "SERVIDOR"

typedef struct 
{
    pid_t pid;
    char texto;
    char FIFOdeResposta[100];
}dataMSG;

int main()
{
    mkfifo("respostaCliente", 0666);

    dataMSG mensagem;
    strcpy(mensagem.FIFOdeResposta, "respostaCliente");
    mensagem.pid = getpid();

    strcpy(mensagem.texto, "ola do cliente");

    int fd = open (SERVER_FIFO, O_WRONLY);
    if (fd == -1){printf("erro\n");}
    int size = write(fd, &mensagem, sizeof(mensagem));
}