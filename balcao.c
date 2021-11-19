// ISEC - Trabalho Prático de POO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){

    int maxMed, maxClt;
    mostrarASCII();
    printf("[BALCÃO]\nBem vindo ao MEDICALso, Administrador");
    printf("\nNúmero máximo de médicos: ");
    scanf("%d", &maxMed);
    printf("\nNúmero máximo de clientes: ");
    scanf("%d", &maxClt);
    fflush(stdout);
    setenv("MAXMEDICOS", maxMed, 1);
    setenv("MAXCLIENTES", maxClt, 1);

    int pid = fork();
    if(pid == 0){
        execvp("./classificador", NULL);
    }
    wait(NULL);
    
    return 0;
}