// ISEC - Trabalho Prático de SO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "medico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){

    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n\n");
    char *mso_state = getenv("MEDICALSO_RUN");
    if(mso_state == NULL){
        printf("[MÉDICO]\nO balcão está fora de serviço\n");
        return 0;
    }
    if(argv[1] == NULL || argv[2] == NULL){
        printf("[MÉDICO]\nPor favor insira um nome e uma especialidade\nUtilização: ./medico [nome] [espcialidade]\n");
        return 0;
    }
    else printf("[MÉDICO]\nBem vindo ao MEDICALso, Dr. %s\nA sua especialidade é %s\n", argv[1], argv[2]);

    return 0;
}