// ISEC - Trabalho Prático de POO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#ifndef CLIENTE
#define CLIENTE
#define MAX 100

void mostrarASCII();

struct Cliente {
    int pid;
    char nome[MAX];
    char sintomas[MAX];
};

#endif //CLIENTE