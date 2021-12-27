// ISEC - Trabalho Prático de SO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "medico.h"
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

#define BALCAO_FIFO "../MEDICALso"
#define MEDICO_FIFO "../MEDICO[%d]"

char MEDICO_FIFO_FINAL[MAX];
int pidBalcao = 0;

int balcaoAberto(){
    int fd_balcao = open(BALCAO_FIFO, O_RDONLY | O_NONBLOCK);
    close(fd_balcao);
    if(fd_balcao == -1){
        return 0;
    }
    return 1;
}

void handler_funcSignal(int signum){ // Para o sa_handler com (completo - PIDs, etc...)
    printf("\nEstou vivo meu puto!\n");
}

void *enviarSinais(void *vargp)
{
    while(1){
        alarm(SINAL_VIDA);
        pause();
    }
}

int main(int argc, char *argv[]){

    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n\n");
    fflush(stdout);
    if(!balcaoAberto()){
        printf("[MÉDICO]\nO balcão está fora de serviço\n");
        return 0;
    }
    if(argv[1] == NULL || argv[2] == NULL){
        printf("[MÉDICO]\nPor favor insira um nome e uma especialidade\nUtilização: ./medico [nome] [espcialidade]\n");
        return 0;
    } else printf("[MÉDICO]\nBem vindo ao MEDICALso, Dr. %s\nA sua especialidade é %s\n", argv[1], argv[2]);
    
    pthread_t thread_id;
    medico m;
    resposta r;

    sprintf(MEDICO_FIFO_FINAL, MEDICO_FIFO, getpid());
    if(mkfifo(MEDICO_FIFO_FINAL,0666) == -1){
        if(errno == EEXIST){
            printf("FIFO ja existe!\n");
        }
        printf("Erro ao abrir fifo!\n");
        return 1;
    }
    
    m.pid = getpid();
    strcpy(m.nome, argv[1]);
    strcpy(m.especialidade, argv[2]);
    int fd_envio = open(BALCAO_FIFO, O_WRONLY);
    int size = write(fd_envio, &m, sizeof(medico));

    // Abrir FIFO para receber pedidos
    int fd_receber = open(MEDICO_FIFO_FINAL, O_RDONLY);
    int response = read(fd_receber, &r, sizeof(resposta));
    pidBalcao = r.pid;
    if(strcmp(r.resposta, "ok!") == 0){
        printf("[MÉDICO]\nPedido aceite!\n");

        struct sigaction sa;
        sa.sa_handler = handler_funcSignal;
        sa.sa_flags = SA_RESTART | SA_SIGINFO;
        sigaction(SIGALRM, &sa, NULL);
        
        pthread_create(&thread_id, NULL, enviarSinais, NULL);
        pthread_join(thread_id, NULL);

        while(1){
            printf("Gang");
        }

    } else {
        printf("[MÉDICO]\nPedido não aceite!\n");
    }
    close(fd_receber);
    close(fd_envio);
    unlink(MEDICO_FIFO_FINAL);

    return 0;
}