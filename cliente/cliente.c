// ISEC - Trabalho Prático de SO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "cliente.h"
#include "../medico/medico.h"
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

void menu(){
    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n");
}

typedef struct Consulta {
    char pipeMedico[MAX];
    char pipeCliente[MAX];
} consulta, *consulta_ptr;

char CLIENTE_FIFO_FINAL[MAX];
char MEDICO_FIFO_FINAL[MAX];
int fd_recebe, fd_envio, fd_medico_w, fd_medico_r;

void fecharCliente(){
    printf("\n[CLIENTE]\nO cliente foi desconectado\n");
    close(fd_recebe);
    close(fd_envio);
    unlink(CLIENTE_FIFO_FINAL);
    exit(0);
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
        printf("\n[CLIENTE]\nO balcão está fora de serviço\n");
        return 0;
    }
    if(argv[1] == NULL || argc != 2){
        printf("\n[CLIENTE]\nPor favor insira um nome\nUtilização: ./cliente [nome]\n");
        return 0;
    }

    cliente c;
    medico m;

    struct sigaction sa;
    sa.sa_handler = fecharCliente;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGINT, &sa, NULL);

    sprintf(CLIENTE_FIFO_FINAL, CLIENTE_FIFO, getpid());
    if(mkfifo(CLIENTE_FIFO_FINAL,0666) == -1){
        printf("\n[CLIENTE]\nOcorreu um erro ao criar um túnel de comunicação!\n");
        unlink(CLIENTE_FIFO_FINAL);
        return 1;
    }

    printf("Introduza os seus sintomas: ");
    scanf("%[^\n]", c.sintomas);
    c.alive = 1;
    c.pid = getpid();
    strcpy(c.pipeCliente, CLIENTE_FIFO_FINAL);
    strcpy(c.nome, argv[1]);
    fd_envio = open(BALCAO_FIFO_CLI, O_WRONLY);
    if(fd_envio == -1){
        printf("\n[CLIENTE]\nOcorreu um erro ao abrir o túnel de comunicação WRITE!\n");
        close(fd_envio);
        unlink(CLIENTE_FIFO_FINAL);
        return 1;
    }
    int size_s = write(fd_envio, &c, sizeof(cliente));
    if(size_s == -1){
        printf("\n[CLIENTE]\nOcorreu um erro ao autenticar-se\n");
        close(fd_envio);
        unlink(CLIENTE_FIFO_FINAL);
        return 1;
    }
    char resposta[MAX];
    fd_recebe = open(CLIENTE_FIFO_FINAL, O_RDONLY);
    if(fd_recebe == -1){
        printf("\n[CLIENTE]\nOcorreu um erro ao abrir o túnel de comunicação READ!\n");
        close(fd_envio);
        close(fd_recebe);
        unlink(CLIENTE_FIFO_FINAL);
        return 1;
    }
    int size = read(fd_recebe, resposta, sizeof(resposta));
    if(size > 0){
        if(!strcmp("ERROR 400 - LIMITE ATINGIDO", resposta))
            printf("\n[CLIENTE]\nNão foi possível conectar ao balcão:\nLimite de pacientes atingido\n");
        else if(!strcmp("SUCCESS 200 - ACEITE", resposta)){
            printf("\n[CLIENTE]\nBem vindo ao MEDICALso, %s\n", c.nome);
            size = read(fd_recebe, &c, sizeof(cliente));
            if(size == -1){
                printf("\n[CLIENTE]\nOcorreu um erro ao receber a análise\n");
                close(fd_envio);
                close(fd_recebe);
                unlink(CLIENTE_FIFO_FINAL);
                return 1;
            }
            printf("Encontra-se na posição %d na fila para a especialidade %s\n", c.posicaoFila, c.analise);
            size = read(fd_recebe, &m, sizeof(medico));
            if(size == -1){
                printf("\n[CLIENTE]\nOcorreu um erro ao receber as informações do médico\n");
                close(fd_envio);
                close(fd_recebe);
                unlink(CLIENTE_FIFO_FINAL);
                return 1;
            }
            menu();
            printf("\n[CLIENTE]\nA ser atendido pelo/a especialista %s\nEspecialidade: %s\n\n---- INÍCIO DA CONVERSA ----\n", m.nome, m.especialidade);
            do{
                strcpy(resposta, "");
                printf("Introduza uma mensagem: ");
                fflush(stdout);
                fflush(stdin);
                scanf("%[^\n]", resposta);
                int fd_medico_w = open(m.pipeMedico, O_WRONLY);
                if(fd_medico_w == -1){
                    printf("\n[CLIENTE]\nOcorreu um erro ao abrir o túnel de comunicação WRITE!\n");
                    close(fd_envio);
                    close(fd_recebe);
                    unlink(CLIENTE_FIFO_FINAL);
                    return 1;
                }
                int size_m = write(fd_medico_w, resposta, MAX);
                if(size_m == -1){
                    printf("\n[CLIENTE]\nOcorreu um erro ao enviar a mensagem\n");
                    close(fd_medico_w);
                    close(fd_recebe);
                    close(fd_envio);
                    unlink(CLIENTE_FIFO_FINAL);
                    return 1;
                }
                printf("%d\n", size_m);
                close(fd_medico_w);
                int fd_medico_r = open(CLIENTE_FIFO_FINAL, O_RDONLY | O_NONBLOCK);
                if(fd_medico_r == -1){
                    printf("\n[CLIENTE]\nOcorreu um erro ao abrir o túnel de comunicação READ!\n");
                    close(fd_envio);
                    close(fd_recebe);
                    unlink(CLIENTE_FIFO_FINAL);
                    return 1;
                }
                int size2 = read(fd_medico_r, resposta, MAX);
                if(size2 > 0){
                    close(fd_medico_r);
                    printf("\n[CLIENTE]\n%s\n", resposta);
                }
            }while(strcmp(resposta, "adeus"));
        }
    } else {
        printf("\n[CLIENTE]\nOcorreu um problema ao receber uma resposta do balcão\n");
    }

    close(fd_medico_r);
    close(fd_medico_w);
    close(fd_recebe);
    close(fd_envio);
    unlink(CLIENTE_FIFO_FINAL);

    return 0;
}