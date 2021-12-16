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
}dataMSG;

int main(){
    dataMSG mensagem;

    if(mkfifo(SERVER_FIFO,0666) == -1){
        if(errno == EEXIST){
            printf("FIFO ja existe\n");
        }
        printf("Erro ao abrir fifo");
        return 1;
    }

    mkfifo(SERVER_FIFO,0666); //só um é que abre
    int fd = open(SERVER_FIFO,O_RDONLY);
    if(fd == -1){printf("erro\n");}
    int size = read(fd, &mensagem,sizeof(mensagem));
    if(size > 0)
        printf("Tamanho [%d] mensagem [%s]",size , mensagem.texto);

    close(fd);
    unlink(SERVER_FIFO);
}