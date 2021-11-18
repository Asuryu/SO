// ISEC - Trabalho Prático de POO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){

    mostrarASCII();
    printf("[CLIENTE]\nBem vindo ao MEDICALso, %s\n", argv[1]);
    fflush(stdout);

    return 0;
}