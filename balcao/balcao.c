// ISEC - Trabalho Prático de POO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "../engine.h"
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
    char *med_env = getenv("MAXMEDICOS");
    char *clt_env = getenv("MAXMEDICOS");
    if(med_env == NULL || clt_env == NULL){
        printf("\nAs variáveis de ambiente não estão definidas.\n");
        return 0;
    }
    int maxMed = atoi(med_env);
    int maxClt = atoi(clt_env);
    printf("\nNúmero máximo de médicos: %d\nNúmero máximo de clientes: %d\n", maxMed, maxClt);
    fflush(stdin);
    fflush(stdout);

    int pid = fork();
    if(pid == 0){
        execvp("../classificador", NULL);
    }
    wait(NULL);
    
    return 0;
}