// ISEC - Trabalho Prático de SO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#ifndef CLIENTE
#define CLIENTE
#define MAX 100
#define SINAL_VIDA 20

#include <stdio.h>

typedef struct Cliente {
    int pid;
    char nome[MAX];
    char sintomas[MAX];
    char analise[MAX];
    int alive;
} cliente, *cliente_ptr;

#endif //CLIENTE