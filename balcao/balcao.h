// ISEC - Trabalho Prático de POO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#ifndef BALCAO
#define BALCAO
#define MAX 100

#include <stdio.h>
#include "../cliente/cliente.h"
#include "../medico/medico.h"

typedef struct Balcao {
    int unpipeBC[2], unpipeCB[2];
    int nClientesMax, nMedicosMax;
    int nClientesAtivos, nMedicosAtivos;
    struct Medico medicos[MAX];
    struct Cliente clientes[MAX];
} balcao, *balcao_ptr;

int startsWith(const char *a, const char *b);

#endif //BALCAO