// ISEC - Trabalho Prático de SO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#ifndef MEDICO
#define MEDICO
#define MAX 100
#define SINAL_VIDA 20

#include <stdio.h>

typedef struct Medico {
    int pid;
    char nome[MAX];
    char especialidade[MAX];
} medico, *medico_ptr;

#endif //MEDICO