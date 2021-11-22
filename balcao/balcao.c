// ISEC - Trabalho Prático de SO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

//FIXME: Quando o pai fecha o classificador continua aberto

#include "balcao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n\n");
    printf("[BALCÃO]\nBem vindo ao MEDICALso, Administrador");
    fflush(stdout);
    char sintomas[MAX];
    char analise[MAX];
    char *med_env = getenv("MAXMEDICOS");
    char *clt_env = getenv("MAXCLIENTES");
    if(med_env == NULL || clt_env == NULL){
        printf("\nAs variáveis de ambiente não estão definidas.\n");
        return 0;
    }
    
    int maxMed = atoi(med_env);
    int maxClt = atoi(clt_env);
    if(maxMed < 1 || maxClt < 1){
        printf("\nAs variáveis de ambiente estão mal definidas.\n");
        return 0;
    }

    printf("\nNúmero máximo de médicos: %d\nNúmero máximo de clientes: %d\n", maxMed, maxClt);
    fflush(stdin);
    fflush(stdout);
// SOU BUEDA FIXEEE
    balcao b; // Struct do tipo balcão
    pipe(b.unpipeBC); // Criação do pipe Balcão -> Classificador
    pipe(b.unpipeCB); // Criação do pipe Classificador -> Balcão

    int pid = fork(); // Criação de um processo filho
    if(pid == 0){ // Código a correr pelo processo filho
        close(STDIN_FILENO); // Fecha o STDIN do processo filho
        close(STDOUT_FILENO); // Fecha o STDOUT do processo filho
        dup(b.unpipeBC[0]); // Cria uma cópia do file descriptor relativo ao read end do pipe Balcão -> Classificador
        dup(b.unpipeCB[1]); // Cria uma cópia do file descriptor relativo ao write end do pipe Classificador -> Balcão
        execl("../classificador", "../classificador", (char*)NULL); // Executa o classificador sem argumentos extra
    } else { // Código a correr pelo processo pai
        close(b.unpipeBC[0]); // Fecha o read do pipe Balcão -> Classificador
        close(b.unpipeCB[1]); // Fecha o write do pipe Classficador -> Balcão
    }
    while (1){ // Ciclo para pedir os sintomas ao utilizador
        strcpy(analise,"");
        printf("\nIndique os seus sintomas (debug): ");
        fgets(sintomas, sizeof(sintomas), stdin);
        sintomas[strlen(sintomas) - 1] = '\0';
        strcat(sintomas, "\n");

        if(!strcmp(sintomas, "#fim\n")) exit(0);
        else if(!strcmp(sintomas, "utentes\n")) printf("A listar todos os utentes...");
        else if(!strcmp(sintomas, "especialistas\n")) printf("A listar todos os especialistas...");
        else if(!strncmp(sintomas, "delut", strlen("delut"))) printf("Utilizador XYZ removido");
        else if(!strncmp(sintomas, "delesp", strlen("delesp"))) printf("Especialista XYZ removido");
        else if(!strncmp(sintomas, "freq", strlen("freq"))) printf("A apresentar a ocupação das filas de X em X segundos...");
        else if(!strcmp(sintomas, "encerra\n")) exit(0);
        else {
            write(b.unpipeBC[1], sintomas, strlen(sintomas)); // Escrever para o pipe Balcão -> Classificador o conteúdo da variável sintomas
            int tmp = read(b.unpipeCB[0], analise, MAX); // Ler para a variável análise o conteúdo existente no pipe Classificador -> Balcão
            analise[tmp-1]= '\0';
            printf("O classificador retornou: %s", analise);
            fflush(stdout);
            fflush(stdin);
        }
    }
    wait(NULL); // Esperar que o processo filho termine
    return 0;
}
