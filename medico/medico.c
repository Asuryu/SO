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
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BALCAO_FIFO "../MEDICALso"
#define BALCAO_FIFO_MED "../MEDICALsoMED"
#define MEDICO_FIFO "../MEDICO[%d]"

typedef struct Vida {
    int pid;
    char tipo[MAX];
} vida, *vida_ptr;

char MEDICO_FIFO_FINAL[MAX];
pthread_t thread_id;
int fd_recebe, fd_envio;

void fecharMedico(int signum){
    printf("\n[MÉDICO]\nO médico foi desconectado\n");
    close(fd_recebe);
    close(fd_envio);
    unlink(MEDICO_FIFO_FINAL);
    exit(0);
}

void enviaSinalVida(int signum){
    vida v;
    v.pid = getpid();
    strcpy(v.tipo, "MÉDICO");
    int fd_balcao = open(BALCAO_FIFO, O_WRONLY | O_NONBLOCK);
    if(fd_balcao == -1){
        printf("[MÉDICO]\nOcorreu um erro ao enviar o sinal de vida!\n");
        fecharMedico(0);
    }
    write(fd_balcao, &v, sizeof(vida));
}

void *threadVida(void *vargp){
    struct sigaction sa;
    sa.sa_handler = enviaSinalVida;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGALRM, &sa, NULL);

    // Enviar sinal de vida a cada 5 segundos
    while (1){
        sleep(SINAL_VIDA);
        kill(getpid(), SIGALRM);
    }
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
        printf("[MÉDICO]\nO balcão está fora de serviço\n");
        return 0;
    }
    if(argv[1] == NULL || argv[2] == NULL){
        printf("[MÉDICO]\nPor favor insira um nome e uma especialidade\nUtilização: ./medico [nome] [espcialidade]\n");
        return 0;
    }
    
    medico m;

    struct sigaction sa;
    sa.sa_handler = fecharMedico;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGINT, &sa, NULL);

    sprintf(MEDICO_FIFO_FINAL, MEDICO_FIFO, getpid());
    if(mkfifo(MEDICO_FIFO_FINAL,0666) == -1){
        printf("[MÉDICO]\nOcorreu um erro ao criar um túnel de comunicação!\n");
        unlink(MEDICO_FIFO_FINAL);
        return 1;
    }
    
    m.pid = getpid();
    m.alive = 1;
    m.ocupado = 0;
    strcpy(m.nome, argv[1]); 
    strcpy(m.especialidade, argv[2]);
    fd_envio = open(BALCAO_FIFO_MED, O_WRONLY);
    if(fd_envio == -1){
        printf("[MÉDICO]\nOcorreu um erro ao abrir o túnel de comunicação WRITE!\n");
        close(fd_envio);
        unlink(MEDICO_FIFO_FINAL);
        return 1;
    }
    int size_s = write(fd_envio, &m, sizeof(medico));
    if(size_s == -1){
        printf("\n[MÉDICO]\nOcorreu um erro ao autenticar-se\n");
        close(fd_envio);
        unlink(MEDICO_FIFO_FINAL);
        return 1;
    }
    char resposta[MAX];
    fd_recebe = open(MEDICO_FIFO_FINAL, O_RDONLY);
    if(fd_recebe == -1){
        printf("[MÉDICO]\nOcorreu um erro ao abrir o túnel de comunicação READ!\n");
        close(fd_envio);
        close(fd_recebe);
        unlink(MEDICO_FIFO_FINAL);
        return 1;
    }
    int size = read(fd_recebe,  resposta, sizeof(resposta));
    if(size > 0){
        if(!strcmp("ERROR 400 - LIMITE ATINGIDO", resposta))
            printf("[MÉDICO]\nNão foi possível conectar ao balcão:\nLimite de médicos atingido\n");
        else if(!strcmp("SUCCESS 200 - ACEITE", resposta)){
            printf("[MÉDICO]\nBem vindo ao MEDICALso, Dr. %s\nA sua especialidade é %s\n", m.nome, m.especialidade);
            // pthread_create(&thread_id, NULL, threadVida, NULL);
            // pthread_join(thread_id, NULL);
        }
        else printf("[MÉDICO]\nOcorreu um problema ao receber uma resposta do balcão\n");
    } else {
        printf("\n[MÉDICO]\nOcorreu um problema ao receber uma resposta do balcão\n");
    }
    
    close(fd_recebe);
    close(fd_envio);
    unlink(MEDICO_FIFO_FINAL);

    return 0;
}