// ISEC - Trabalho Prático de SO 2021/2022
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
    int nClientesEspera;
    struct Medico medicos[MAX];
    struct Cliente clientes[MAX];
    struct Cliente clienteEspera[MAX];
} balcao, *balcao_ptr;

#endif //BALCAO