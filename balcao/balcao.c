// ISEC - Trabalho Prático de POO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "balcao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){

    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n\n");
    printf("[BALCÃO]\nBem vindo ao MEDICALso, Administrador");
    fflush(stdout);
    char command[MAX];
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

    printf("\nNúmero máximo de médicos: %d\nNúmero máximo de clientes: %d", maxMed, maxClt);
    fflush(stdin);
    fflush(stdout);

    balcao b;
    int retBC = pipe(b.unpipeBC);
    int retCB = pipe(b.unpipeCB);

    int pid = fork();
    if(pid == 0){
        close(STDIN_FILENO);
        dup(b.unpipeBC[0]);
        close(b.unpipeBC[0]);
        close(b.unpipeBC[1]);
        close(STDOUT_FILENO);
        dup(b.unpipeCB[1]);
        close(b.unpipeCB[1]);
        close(b.unpipeCB[0]);
        execvp("../classificador", NULL);
    } else {
        close(b.unpipeBC[0]);
        close(b.unpipeCB[1]);
    }
    while (1){
        strcpy(analise,"");
        printf("\nIndique os seus sintomas (debug): ");
        fgets(sintomas, sizeof(sintomas), stdin);
        sintomas[strlen(sintomas) - 1] = '\0';
        strcat(sintomas, "\n");
        write(b.unpipeBC[1], sintomas, strlen(sintomas));
        int tmp = read(b.unpipeCB[0], analise, MAX);
        analise[tmp-1]= '\0';
        printf("O classificador retornou: %s", analise);
        fflush(stdout);
        fflush(stdin);
    }
    wait(NULL);
    return 0;
}