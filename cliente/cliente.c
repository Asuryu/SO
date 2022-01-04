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

typedef struct Consulta {
    char pipeMedico[MAX];
    char pipeCliente[MAX];
} consulta, *consulta_ptr;

char CLIENTE_FIFO_FINAL[MAX];
int fd_recebe, fd_envio;

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

    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n");
    if(!balcaoAberto()){
        printf("\n[CLIENTE]\nO balcão está fora de serviço\n");
        return 0;
    }
    if(argv[1] == NULL || argc != 2){
        printf("\n[CLIENTE]\nPor favor insira um nome\nUtilização: ./cliente [nome]\n");
        return 0;
    }

    cliente c;

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
            read(fd_recebe, &c, sizeof(cliente));
            printf("Encontra-se na posição %d na fila para a especialidade %s\n", c.posicaoFila, c.analise);
            read(fd_recebe, resposta, sizeof(resposta));
        }
    } else {
        printf("\n[CLIENTE]\nOcorreu um problema ao receber uma resposta do balcão\n");
    }

    close(fd_recebe);
    close(fd_envio);
    unlink(CLIENTE_FIFO_FINAL);

    return 0;
}