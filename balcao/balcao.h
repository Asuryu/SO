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
    struct Cliente oftalmologia[4], neurologia[4], estomatologia[4], ortopedia[4], geral[4];
    int nClientesOftalmologia, nClientesNeurologia, nClientesEstomatologia, nClientesOrtopedia, nClientesGeral;
} balcao, *balcao_ptr;

typedef struct Vida {
    int pid;
    char tipo[MAX];
} vida, *vida_ptr;

typedef struct Consulta {
    char pipeMedico[MAX];
    char pipeCliente[MAX];
} consulta, *consulta_ptr;

#endif //BALCAO