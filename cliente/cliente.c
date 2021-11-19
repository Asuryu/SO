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
    if(argv[1] == NULL) printf("[CLIENTE]\nPor favor insira um nome\nUtilização: ./cliente [nome]\n");
    else printf("[CLIENTE]\nBem vindo ao MEDICALso, %s\n", argv[1]);
    
    fflush(stdout);

    return 0;
}