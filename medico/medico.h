// ISEC - Trabalho Prático de POO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#ifndef MEDICO
#define MEDICO
#define MAX 100

#include <stdio.h>

struct Medico {
    int pid;
    char nome[MAX];
    char especialidade[MAX];
};

#endif //MEDICO