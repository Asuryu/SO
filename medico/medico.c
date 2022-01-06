// ISEC - Trabalho Prático de SO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "medico.h"
#include "../cliente/cliente.h"
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
#include <sys/select.h>

#define BALCAO_FIFO "../MEDICALso"
#define BALCAO_FIFO_MED "../MEDICALsoMED"
#define MEDICO_FIFO "../MEDICO[%d]"

void menu(){
    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n\n");
}

typedef struct Vida {
    int pid;
    char tipo[MAX];
} vida, *vida_ptr;

typedef struct Consulta {
    char pipeMedico[MAX];
    char pipeCliente[MAX];
} consulta, *consulta_ptr;

char MEDICO_FIFO_FINAL[MAX];
char CLIENTE_FIFO_FINAL[MAX];
pthread_t thread_id;
int fd_recebe, fd_envio, fd_cliente_r, fd_cliente_w;
fd_set read_fds;
struct timeval tv;

void fecharMedico(){
    printf("\n[MÉDICO]\nO médico foi desconectado\n");
    close(fd_recebe);
    close(fd_envio);
    unlink(MEDICO_FIFO_FINAL);
    exit(0);
}

void enviaSinalVida(){
    vida v;
    v.pid = getpid();
    strcpy(v.tipo, "MÉDICO");
    int fd_balcao = open(BALCAO_FIFO, O_WRONLY | O_NONBLOCK);
    if(fd_balcao == -1){
        printf("[MÉDICO]\nOcorreu um erro ao enviar o sinal de vida!\n");
        fecharMedico();
    }
    int size = write(fd_balcao, &v, sizeof(vida));
    if(size == -1){
        printf("[MÉDICO]\nOcorreu um erro ao enviar o sinal de vida!\n");
        fecharMedico();
    }
}

void *threadVida(){
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

    menu();
    if(!balcaoAberto()){
        printf("[MÉDICO]\nO balcão está fora de serviço\n");
        return 0;
    }
    if(argv[1] == NULL || argv[2] == NULL || argc != 3){
        printf("[MÉDICO]\nPor favor insira um nome e uma especialidade\nUtilização: ./medico [nome] [espcialidade]\n");
        return 0;
    }
    // Apenas aceitar especialidades (oftalmologia, ortopedia, geral, estomatologia e neurologia)
    if(strcmp(argv[2], "oftalmologia") != 0 && strcmp(argv[2], "ortopedia") != 0 && strcmp(argv[2], "geral") != 0 && strcmp(argv[2], "estomatologia") != 0 && strcmp(argv[2], "neurologia") != 0){
        printf("[MÉDICO]\nA especialidade inserida não é válida\n");
        return 0;
    }
    
    medico m;
    cliente c;

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
    strcpy(m.pipeMedico, MEDICO_FIFO_FINAL);
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
            size = read(fd_recebe, &c, sizeof(cliente));
            close(fd_recebe);
            if(size == -1){
                printf("[MÉDICO]\nOcorreu um erro ao receber o cliente!\n");
                close(fd_envio);
                close(fd_recebe);
                unlink(MEDICO_FIFO_FINAL);
                return 1;
            }
            menu();
            fflush(stdout);
            printf("[MÉDICO]\nA atender o/a utente %s\nSintomas: %s\n\n---- INÍCIO DA CONVERSA ----\n", c.nome, c.sintomas);
            int pipeCliente = open(c.pipeCliente, O_WRONLY);
            if(pipeCliente == -1){
                printf("[MÉDICO]\nOcorreu um erro ao abrir o túnel de comunicação WRITE!\n");
                close(fd_envio);
                close(fd_recebe);
                unlink(MEDICO_FIFO_FINAL);
                return 1;
            }
            int size = write(pipeCliente, &m, sizeof(medico));
            if(size == -1){
                printf("[MÉDICO]\nOcorreu um erro ao enviar o médico ao utente!\n");
                close(fd_envio);
                close(fd_recebe);
                close(pipeCliente);
                unlink(MEDICO_FIFO_FINAL);
                return 1;
            }
            close(pipeCliente);
            char resposta[MAX];
            printf("\nIntroduza uma mensagem: ");
                    fflush(stdout);
                    fflush(stdin);
                    scanf("%[^\n]", resposta);
            int fd_cliente_r = open(MEDICO_FIFO_FINAL, O_RDONLY | O_NONBLOCK);
            if(fd_cliente_r == -1){
                printf("[MÉDICO]\nOcorreu um erro ao abrir o túnel de comunicação READ!\n");
                close(fd_envio);
                close(fd_recebe);
                close(fd_cliente_r);
                unlink(MEDICO_FIFO_FINAL);
                return 1;
            }
            do{
                tv.tv_sec = 100;
                tv.tv_usec = 0;
                FD_ZERO(&read_fds);
                FD_SET(0, &read_fds);
                FD_SET(fd_cliente_r, &read_fds);
                select(fd_cliente_r + 1, &read_fds, NULL, NULL, &tv);
                int size = read(fd_cliente_r, resposta, MAX);
                if(size == -1){
                    printf("[MÉDICO]\nOcorreu um erro ao receber a resposta do utente!\n");
                    close(fd_envio);
                    close(fd_recebe);
                    close(fd_cliente_r);
                    unlink(MEDICO_FIFO_FINAL);
                    return 1;
                }
                if(FD_ISSET(0, &read_fds)){
                    strcpy(resposta, "");
                    printf("\nIntroduza uma mensagem: ");
                    fflush(stdout);
                    fflush(stdin);
                    scanf("%[^\n]", resposta);
                    int fd_client_w = open(c.pipeCliente, O_WRONLY);
                    write(fd_client_w, resposta, sizeof(resposta));
                    close(fd_client_w);
                }
                if(FD_ISSET(fd_cliente_r, &read_fds)){
                    if(size > 0){
                        printf("\n%s: %s\n", c.nome, resposta);
                        if(!strcmp(resposta, "adeus")){
                            printf("\n---- FIM DA CONVERSA ----\n");
                            close(fd_cliente_r);
                            break;
                        }
                    }
                }
            } while(1);

            close(fd_cliente_r);

            pthread_create(&thread_id, NULL, threadVida, NULL);
            pthread_join(thread_id, NULL);
        }
        else printf("[MÉDICO]\nOcorreu um problema ao receber uma resposta do balcão\n");
    } else {
        printf("\n[MÉDICO]\nOcorreu um problema ao receber uma resposta do balcão\n");
    }
    
    close(fd_cliente_r);
    close(fd_cliente_w);
    close(fd_recebe);
    close(fd_envio);
    unlink(MEDICO_FIFO_FINAL);

    return 0;
}