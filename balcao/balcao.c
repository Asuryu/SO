// ISEC - Trabalho Prático de SO 2021/2022
// Tomás Gomes Silva - 2020143845
// Tomás da Cunha Pinto - 2020144067

#include "balcao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


#define BALCAO_FIFO "../MEDICALso"
#define BALCAO_FIFO_MED "../MEDICALsoMED"
#define BALCAO_FIFO_CLI "../MEDICALsoCLI"
#define CLIENTE_FIFO "../CLIENTE[%d]"
#define MEDICO_FIFO "../MEDICO[%d]"

pthread_t thread_id;
pthread_t thread_freq;
char FIFO_FINAL[MAX];
char **args;
balcao b;
int delay = 0;

int onlyBalcao(){
    int fd_balcao = open(BALCAO_FIFO, O_RDONLY | O_NONBLOCK);
    close(fd_balcao);
    if(fd_balcao == -1){
        return 1;
    }
    return 0;
}

void mostrarFilas(){
    if(b.nClientesOftalmologia > 0){
        printf("\n\n[BALCÃO] Existem %d utentes em espera para OFTALMOLOGIA:\n", b.nClientesOftalmologia);
        for(int i = 0; i < b.nClientesOftalmologia; i++){
            printf("Cliente %s com PID %d em espera para %s\n", b.oftalmologia[i].nome, b.oftalmologia[i].pid, b.oftalmologia[i].analise);
        }
    } else printf("\n\n[BALCÃO] Não há utentes em espera para OFTALMOLOGIA\n");
    if(b.nClientesEstomatologia > 0){
        printf("\n\n[BALCÃO] Existem %d utentes em espera para ESTOMATOLOGIA:\n", b.nClientesEstomatologia);
        for(int i = 0; i < b.nClientesEstomatologia; i++){
            printf("Cliente %s com PID %d em espera para %s\n", b.estomatologia[i].nome, b.estomatologia[i].pid, b.estomatologia[i].analise);
        }
    } else printf("\n\n[BALCÃO] Não há utentes em espera para ESTOMATOLOGIA\n");
    if(b.nClientesNeurologia > 0){
        printf("\n\n[BALCÃO] Existem %d utentes em espera para NEUROLOGIA:\n", b.nClientesNeurologia);
        for(int i = 0; i < b.nClientesNeurologia; i++){
            printf("Cliente %s com PID %d em espera para %s\n", b.neurologia[i].nome, b.neurologia[i].pid, b.neurologia[i].analise);
        }
    } else printf("\n\n[BALCÃO] Não há utentes em espera para NEUROLOGIA\n");
    if(b.nClientesOrtopedia > 0){
        printf("\n\n[BALCÃO] Existem %d utentes em espera para ORTOPEDIA:\n", b.nClientesOrtopedia);
        for(int i = 0; i < b.nClientesOrtopedia; i++){
            printf("Cliente %s com PID %d em espera para %s\n", b.ortopedia[i].nome, b.ortopedia[i].pid, b.ortopedia[i].analise);
        }
    } else printf("\n\n[BALCÃO] Não há utentes em espera para ORTOPEDIA\n");
    if(b.nClientesGeral > 0){
        printf("\n\n[BALCÃO] Existem %d utentes em espera para GERAL:\n", b.nClientesGeral);
        for(int i = 0; i < b.nClientesGeral; i++){
            printf("Cliente %s com PID %d em espera para %s\n", b.geral[i].nome, b.geral[i].pid, b.geral[i].analise);
        }
    } else printf("\n\n[BALCÃO] Não há utentes em espera para GERAL\n");
}

void *updateVivos(){
    // Ler do pipe MEDICALso e atualizar a lista de vivos
    int fd_balcao = open(BALCAO_FIFO, O_RDONLY | O_NONBLOCK);
    if(fd_balcao == -1){
        printf("[BALCAO]\nOcorreu um erro ao abrir o FIFO de leitura!\n");
        return NULL;
    }
    while(1){
        vida v;
        int size = read(fd_balcao, &v, sizeof(vida));
        if(size > 0){
            if(v.pid == 0){
                break;
            }
            if(strcmp(v.tipo, "MÉDICO") == 0){
                for(int i = 0; i < b.nMedicosAtivos; i++){
                    if(b.medicos[i].pid == v.pid){
                        b.medicos[i].alive = 1;
                        break;
                    }
                }
            } else if(strcmp(v.tipo, "CLIENTE") == 0){
                for(int i = 0; i < b.nClientesAtivos; i++){
                    if(b.clientes[i].pid == v.pid){
                        b.clientes[i].alive = 1;
                        break;
                    }
                }
            }
        }
    }
    return NULL;
}

void *removerMortos(){
    while(1){
        sleep(SINAL_VIDA);
        for(int i = 0; i < b.nMedicosAtivos; i++){
            if(b.medicos[i].alive == 0){
                for(int j = i; j < b.nMedicosAtivos - 1; j++){
                    b.medicos[j] = b.medicos[j+1];
                }
                b.nMedicosAtivos--;
                i--;
            }
        }
        // Colocar todos os médicos alive a 0
        for(int i = 0; i < b.nMedicosAtivos; i++){
            b.medicos[i].alive = 0;
        }
    }
}

void *aceitarMedicos(){

    medico m;

    int fdR = open(BALCAO_FIFO_MED, O_RDONLY | O_NONBLOCK);
    if(fdR == -1){
        printf("\n[BALCÃO]\nOcorreu um erro ao abrir o pipe BALCAO_FIFO_MED\n");
        return NULL;
    }
    do
    {   
        int flag = 0;
        int size = read(fdR, &m, sizeof(m));
        if(size > 0){
            for(int i = 0; i < b.nMedicosAtivos; i++){
                if(m.pid == b.medicos[i].pid){
                    flag = 1;
                    break;
                }
            }
            if(b.nMedicosAtivos < b.nMedicosMax && flag == 0 && m.pid != 0){
                b.medicos[b.nMedicosAtivos] = m;
                b.nMedicosAtivos++;
                printf("\n[PID %d] Médico: %s (%s)\n", m.pid, m.nome, m.especialidade);
                
                sprintf(FIFO_FINAL, MEDICO_FIFO, m.pid); //Guarda no "FIFO_FINAL" o nome do pipe para onde queremos enviar as cenas
                int fd_envio = open(FIFO_FINAL, O_WRONLY);
                if(fd_envio == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao abrir um pipe com o médico com PID %d\n", m.pid);
                }
                int size2 = write(fd_envio, "SUCCESS 200 - ACEITE", sizeof("SUCCESS 200 - ACEITE"));
                if(size2 == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao enviar mensagem de estado ao médico com PID %d\n", m.pid);
                }

                if(strcmp(m.especialidade, "oftalmologia") == 0){
                    if(b.nClientesOftalmologia > 0){
                        int size2 = write(fd_envio, &b.oftalmologia[0], sizeof(cliente));
                        b.medicos[b.nMedicosAtivos].ocupado = 1;
                        if(size2 == -1){
                            printf("\n[BALCÃO] Ocorreu um erro ao enviar mensagem de estado ao cliente com PID %d\n", b.oftalmologia[0].pid);
                        }
                    }

                } else if(strcmp(m.especialidade, "neurologia") == 0){
                    // if(b.nClientesNeurologia > 0){
                } else if(strcmp(m.especialidade, "estomatologia") == 0){
                    // if(b.nClientesNeurologia > 0){
                } else if(strcmp(m.especialidade, "ortopedia") == 0){
                    // if(b.nClientesNeurologia > 0){
                } else if(strcmp(m.especialidade, "geral") == 0){
                    // if(b.nClientesNeurologia > 0){
                }

            } else {
                // Médico não aceite
                printf("\n[PID %d] Médico: %s (%s) --> Não aceite\n", m.pid, m.nome, m.especialidade);
                
                sprintf(FIFO_FINAL, MEDICO_FIFO, m.pid); //Guarda no "FIFO_FINAL" o nome do pipe para onde queremos enviar as cenas
                int fd_envio = open(FIFO_FINAL, O_WRONLY);
                if(fd_envio == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao abrir um pipe com o médico com PID %d\n", m.pid);
                }
                int size2 = write(fd_envio, "ERROR 400 - LIMITE ATINGIDO", sizeof("ERROR 400 - LIMITE ATINGIDO"));
                if(size2 == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao enviar mensagem de estado ao médico com PID %d\n", m.pid);
                }
                
            }
        }

    } while (1);
}

void *aceitarClientes(){

    cliente c;
    char resposta[MAX];

    int fdR = open(BALCAO_FIFO_CLI, O_RDONLY | O_NONBLOCK);
    if(fdR == -1){
        printf("\n[BALCÃO]\nOcorreu um erro ao abrir o pipe BALCAO_FIFO_CLI\n");
        return NULL;
    }
    do
    {   
        int flag = 0;
        int size = read(fdR, &c, sizeof(c));
        if(size > 0){
            for(int i = 0; i < b.nClientesAtivos; i++){
                if(c.pid == b.clientes[i].pid){
                    flag = 1;
                    break;
                }
            }
            if(b.nClientesAtivos < b.nClientesMax && flag == 0 && c.pid != 0){
                b.clientes[b.nClientesAtivos] = c;
                b.nClientesAtivos++;
                printf("\n[PID %d] Cliente: %s (%s)\n", c.pid, c.nome, c.sintomas);
                
                sprintf(FIFO_FINAL, CLIENTE_FIFO, c.pid); // Guarda no "FIFO_FINAL" o nome do pipe para onde queremos enviar as cenas
                int fd_envio = open(FIFO_FINAL, O_WRONLY);
                if(fd_envio == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao abrir um pipe com o cliente com PID %d\n", c.pid);
                }
                int size2 = write(fd_envio, "SUCCESS 200 - ACEITE", sizeof("SUCCESS 200 - ACEITE"));
                if(size2 == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao enviar mensagem de estado ao cliente com PID %d\n", c.pid);
                }

                strcpy(resposta, "");
                strcat(c.sintomas, "\n");
                int size_w = write(b.unpipeBC[1], c.sintomas, strlen(c.sintomas));
                if(size_w == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao enviar mensagem de sintomas ao balcão\n");
                }
                int size3 = read(b.unpipeCB[0], resposta, MAX);
                if(size3 == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao ler o pipe do cliente com PID %d\n", c.pid);
                }
                resposta[strlen(resposta)-1] = '\0';
                strcpy(c.analise, resposta);
                
                char especialidade[MAX];
                strcpy(especialidade, "");
                strcat(especialidade, c.analise);
                especialidade[strlen(especialidade)-2] = '\0';
                c.prioridade = c.analise[strlen(c.analise)-1] - '0';
                
                if(!strcmp(especialidade, "oftalmologia")){
                    b.oftalmologia[b.nClientesOftalmologia] = c;
                    b.nClientesOftalmologia++;
                    // Ordenar todos os utentes por prioridade
                    for(int i = 0; i < b.nClientesOftalmologia-1; i++){
                        for(int j = i+1; j < b.nClientesOftalmologia; j++){
                            if(b.oftalmologia[i].prioridade > b.estomatologia[j].prioridade){
                                cliente aux = b.oftalmologia[i];
                                b.oftalmologia[i] = b.oftalmologia[j];
                                b.oftalmologia[j] = aux;
                                c.posicaoFila = b.oftalmologia[i].posicaoFila = j;
                            }
                        }
                    }
                }
                else if(!strcmp(especialidade, "neurologia")){
                    b.neurologia[b.nClientesNeurologia] = c;
                    b.nClientesNeurologia++;
                    c.posicaoFila = b.nClientesNeurologia;
                }
                else if(!strcmp(especialidade, "estomatologia")){
                    b.estomatologia[b.nClientesEstomatologia] = c;
                    b.nClientesEstomatologia++;
                    c.posicaoFila = b.nClientesEstomatologia;
                }
                else if(!strcmp(especialidade, "ortopedia")){
                    b.ortopedia[b.nClientesOrtopedia] = c;
                    b.nClientesOrtopedia++;
                    c.posicaoFila = b.nClientesOrtopedia;
                }
                else{
                    b.geral[b.nClientesGeral] = c;
                    b.nClientesGeral++;
                    c.posicaoFila = b.nClientesGeral;
                }

                size_w = write(fd_envio, &c, sizeof(cliente));
                if(size_w == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao enviar análise ao cliente\n");
                }

            } else {
                printf("\n[PID %d] Cliente: %s (%s) --> Não aceite\n", c.pid, c.nome, c.sintomas);

                sprintf(FIFO_FINAL, CLIENTE_FIFO, c.pid); // Guarda no "FIFO_FINAL" o nome do pipe para onde queremos enviar as cenas
                int fd_envio = open(FIFO_FINAL, O_WRONLY);
                if(fd_envio == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao abrir um pipe com o cliente com PID %d\n", c.pid);
                }
                int size2 = write(fd_envio, "ERROR 400 - LIMITE ATINGIDO", sizeof("ERROR 400 - LIMITE ATINGIDO"));
                if(size2 == -1){
                    printf("\n[BALCÃO] Ocorreu um erro ao enviar mensagem de estado ao cliente com PID %d\n", c.pid);
                }
            }
        }
    } while (1);
}

void *TemporizadorAlarme(){

    while(1){
        mostrarFilas();
        sleep(delay);
    }
    
};

void *consolaAdministrador(){

    char sintomas[MAX];
    char sintomasFinais[MAX];
    char analise[MAX];

    while (1){ // Ciclo para pedir os sintomas ao utilizador
        strcpy(analise,"");
        strcpy(sintomas,"");
        strcpy(sintomasFinais,"");

        // Get sintomas do utilizador
        // Prevent user from inputting blank lines
        printf("\nIntroduza um comando: ");
        do{
            fgets(sintomas, MAX, stdin);
            if(strcmp(sintomas, "\n") == 0){
                printf("Introduza um comando: ");
            }
        } while (strcmp(sintomas, "\n") == 0);
        sintomas[strlen(sintomas) - 1] = '\0';
        strcpy(sintomasFinais, sintomas);
        
        // Separar a string sintomas por espaços em várias strings e guardar no vetor args (strtok)
        args = (char**)malloc(sizeof(char*)*MAX);
        int i = 0;
        char *token = strtok(sintomas, " ");
        while(token != NULL){
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        if(!strcmp(sintomas, "#fim")) break;
        else if(!strcmp(args[0], "utentes")){
            if(i != 1) printf("\n[BALCÃO] O comando 'utentes' não requer argumentos adicionais");
            else {
                printf("\n[BALCÃO] A listar todos os utentes (%d):\n", b.nClientesAtivos);
                for(int i=0; i < b.nClientesAtivos; i++)
                {
                    printf("Utente [%d] %s com o sintoma %s está em consulta\n", b.clientes[i].pid, b.clientes[i].nome, b.clientes[i].sintomas);
                }
                mostrarFilas();
            }
        }
        else if(!strcmp(args[0], "especialistas")){
            if(i != 1) printf("\n[BALCÃO] O comando 'especialistas' não requer argumentos adicionais");
            else{
                printf("\n[BALCÃO] A listar todos os especialistas (%d):\n", b.nMedicosAtivos);
                for(int i=0; i < b.nMedicosAtivos; i++)
                {
                    printf("Médico [%d] %s com a especialidade %s\n", b.medicos[i].pid, b.medicos[i].nome, b.medicos[i].especialidade);
                }
                if(b.nMedicosAtivos == b.nMedicosMax)
                    printf("\nTodos os médicos encontram-se em serviço.");
            }
        }
        else if(!strcmp(args[0], "delut")){
            if(i != 2) printf("\n[BALCÃO] O comando 'delut' requer apenas um argumento (PID)");
            else{
                int flag = 0;
                char esp[MAX];
                int pid = atoi(args[1]);
                if(pid != 0){
                    int j = 0;
                    for(int i=0; i < b.nClientesEstomatologia; i++){
                        if(b.estomatologia[i].pid == pid){
                            j = i;
                            strcpy(esp, "estomatologia");
                            flag = 1;
                            break;
                        }
                    }
                    for(int i=0; i < b.nClientesGeral; i++){
                        if(b.geral[i].pid == pid){
                            j = i;
                            strcpy(esp, "geral");
                            flag = 1;
                            break;
                        }
                    }
                    for(int i=0; i < b.nClientesNeurologia; i++){
                        if(b.neurologia[i].pid == pid){
                            j = i;
                            strcpy(esp, "neurologia");
                            flag = 1;
                            break;
                        }
                    }
                    for(int i=0; i < b.nClientesOrtopedia; i++){
                        if(b.ortopedia[i].pid == pid){
                            j = i;
                            strcpy(esp, "ortopedia");
                            flag = 1;
                            break;
                        }
                    }
                    for(int i=0; i < b.nClientesOftalmologia; i++){
                        if(b.oftalmologia[i].pid == pid){
                            j = i;
                            strcpy(esp, "oftalmologia");
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 0) printf("\n[BALCÃO] O utente com o PID %d não existe ou não se encontra em espera", pid);
                    else{
                        printf("\n[BALCÃO] O utente com o PID %d foi removido", pid);
                        if(!strcmp(esp, "estomatologia")){
                            b.estomatologia[j] = b.estomatologia[b.nClientesEstomatologia - 1];
                            b.nClientesEstomatologia--;
                        }
                        else if(!strcmp(esp, "geral")){
                            b.geral[j] = b.geral[b.nClientesGeral - 1];
                            b.nClientesGeral--;
                        }
                        else if(!strcmp(esp, "neurologia")){
                            b.neurologia[j] = b.neurologia[b.nClientesNeurologia - 1];
                            b.nClientesNeurologia--;
                        }
                        else if(!strcmp(esp, "ortopedia")){
                            b.ortopedia[j] = b.ortopedia[b.nClientesOrtopedia - 1];
                            b.nClientesOrtopedia--;
                        }
                        else if(!strcmp(esp, "oftalmologia")){
                            b.oftalmologia[j] = b.oftalmologia[b.nClientesOftalmologia - 1];
                            b.nClientesOftalmologia--;
                        }
                        kill(pid, SIGINT);
                    }
                } else printf("\n[BALCÃO] Introduza um número válido");
            }
        }
        else if(!strcmp(args[0], "delesp")){
            if(i != 2) printf("\n[BALCÃO] O comando 'delesp' requer apenas um argumento (PID)");
            else{
                int flag = 0;
                int pid = atoi(args[1]);
                if(pid != 0){
                    int j = 0;
                    for(int i=0; i < b.nMedicosAtivos; i++){
                        if(b.medicos[i].pid == pid && b.medicos[i].ocupado == 0){
                            j = i;
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 0 || b.medicos[j].ocupado == 1) printf("\n[BALCÃO] O médico com o PID %d não existe ou está numa consulta", pid);
                    else{
                        printf("\n[BALCÃO] O médico com o PID %d foi removido", pid);
                        b.medicos[j] = b.medicos[b.nMedicosAtivos - 1];
                        b.nMedicosAtivos--;
                        kill(pid, SIGINT);
                    }
                } else printf("\n[BALCÃO] Introduza um número válido");
            }
        }
        else if(!strcmp(args[0], "freq")){
            if(i != 2) printf("\n[BALCÃO] O comando 'freq' requer apenas um argumento (segundos)");
            else {
                int seconds = atoi(args[1]);
                if(seconds != 0){
                    printf("\n[BALCÃO] A apresentar a ocupação das filas de %d em %d segundos...", seconds, seconds);
                    printf("\nEstão %d utentes em lista de espera", b.nClientesOftalmologia + b.nClientesOrtopedia + b.nClientesNeurologia + b.nClientesGeral + b.nClientesEstomatologia); 
                    delay = seconds;
                    pthread_cancel(thread_freq);
                    pthread_create(&thread_freq, NULL, TemporizadorAlarme, NULL);

                } else {
                    printf("\n[BALCÃO] Introduza um número válido");
                }
            }
        }
        else if(!strcmp(args[0], "encerra")){
            if(i != 1) printf("\n[BALCÃO] O comando 'encerra' não requer argumentos adicionais");
            else{
                break;
            }
        }
        else if(!strcmp(args[0], "clear")){
            if(i != 1) printf("\n[BALCÃO] O comando 'clear' não requer argumentos adicionais");
            else{
                printf("\033[2J\033[1;1H");
                printf("     __ __   __          __ __ \n");
                printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
                printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n");
            }
        }
        else printf("\n[BALCÃO] Comando inválido");
        free(args);
    }
    int size_w = write(b.unpipeBC[1], "#fim\n", strlen("#fim\n"));
    if(size_w < 0) printf("\n[BALCÃO] Erro ao enviar mensagem para o classificador");
    printf("\n[BALCÃO] A encerrar o balcão...\n");

    for(int i = 0; i < b.nMedicosAtivos; i++)
        kill(b.medicos[i].pid, SIGINT);
    for(int i = 0; i < b.nClientesAtivos; i++)
        kill(b.clientes[i].pid, SIGINT);
    for(int i = 0; i < b.nClientesGeral; i++)
        kill(b.geral[i].pid, SIGINT);
    for(int i = 0; i < b.nClientesNeurologia; i++)
        kill(b.neurologia[i].pid, SIGINT);
    for(int i = 0; i < b.nClientesOftalmologia; i++)
        kill(b.oftalmologia[i].pid, SIGINT);
    for(int i = 0; i < b.nClientesOrtopedia; i++)
        kill(b.ortopedia[i].pid, SIGINT);
    for(int i = 0; i < b.nClientesEstomatologia; i++)
        kill(b.estomatologia[i].pid, SIGINT);

    wait(NULL); // Esperar que o processo filho termine
    close(b.unpipeBC[1]); // Fecha o write do pipe Balcão -> Classificador
    close(b.unpipeCB[0]); // Fecha o read do pipe Classificador -> Balcão 
    free(args);
    return NULL;
}

int main(){

    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n");
    printf("\n[BALCÃO]\nBem vindo ao MEDICALso, Administrador");
    fflush(stdout);

    char *med_env = getenv("MAXMEDICOS");
    char *clt_env = getenv("MAXCLIENTES");

    if (!onlyBalcao()) {
        printf("\nJá existe um balcão a correr.\n");
        return 0;
    }
    if(med_env == NULL || clt_env == NULL){
        printf("\nAs variáveis de ambiente não estão definidas.\n");
        return 0;
    }
    int maxMed = atoi(med_env);
    int maxClt = atoi(clt_env); 
    if(maxMed < 1 || maxClt < 1){
        printf("\nAs variáveis de ambiente estão mal definidas.\n");
        return 0;
    }
    b.nMedicosMax = maxMed;
    b.nClientesMax = maxClt;

    printf("\nNúmero máximo de médicos: %d\nNúmero máximo de clientes: %d\n\n", b.nMedicosMax, b.nClientesMax);
    fflush(stdin);
    fflush(stdout);

    // Criação do FIFO do Balcão
    if(mkfifo(BALCAO_FIFO,0666) == -1){
        printf("\n[BALCÃO] Ocorreu um erro ao criar o pipe BALCAO_FIFO!\n");
        return 0;
    }
    if(mkfifo(BALCAO_FIFO_MED,0666) == -1){
        printf("\n[BALCÃO] Ocorreu um erro ao criar o pipe BALCAO_FIFO_MED!\n");
        return 0;
    }
    if(mkfifo(BALCAO_FIFO_CLI,0666) == -1){
        printf("\n[BALCÃO] Ocorreu um erro ao criar o pipe BALCAO_FIFO_CLI!\n");
        return 0;
    }

    pipe(b.unpipeBC); // Criação do pipe Balcão -> Classificador
    pipe(b.unpipeCB); // Criação do pipe Classificador -> Balcão

    int pid = fork(); // Criação de um processo filho
    if(pid == 0){ // Código a correr pelo processo filho
        close(STDIN_FILENO); // Fecha o STDIN do processo filho
        close(STDOUT_FILENO); // Fecha o STDOUT do processo filho
        dup(b.unpipeBC[0]); // Cria uma cópia do file descriptor relativo ao read end do pipe Balcão -> Classificador
        dup(b.unpipeCB[1]); // Cria uma cópia do file descriptor relativo ao write end do pipe Classificador -> Balcão
        execl("../classificador", "../classificador", (char*)NULL); // Executa o classificador sem argumentos extra
    } else { // Código a correr pelo processo pai
        close(b.unpipeBC[0]); // Fecha o read do pipe Balcão -> Classificador
        close(b.unpipeCB[1]); // Fecha o write do pipe Classficador -> Balcão
    }

    if(pthread_create(&thread_id, NULL, aceitarMedicos, NULL)){
        printf("\n[BALCÃO] Ocorreu um erro ao criar a thread aceitarMedicos!\n");
        unlink(BALCAO_FIFO);
        unlink(BALCAO_FIFO_MED);
        unlink(BALCAO_FIFO_CLI);
        return 0;
    }
    if(pthread_create(&thread_id, NULL, aceitarClientes, NULL)){
        printf("\n[BALCÃO] Ocorreu um erro ao criar a thread aceitarClientes!\n");
        unlink(BALCAO_FIFO);
        unlink(BALCAO_FIFO_MED);
        unlink(BALCAO_FIFO_CLI);
        return 0;
    }
    if(pthread_create(&thread_id, NULL, updateVivos, NULL)){
        printf("\n[BALCÃO] Ocorreu um erro ao criar a thread updateVivos!\n");
        unlink(BALCAO_FIFO);
        unlink(BALCAO_FIFO_MED);
        unlink(BALCAO_FIFO_CLI);
        return 0;
    }
    if(pthread_create(&thread_id, NULL, removerMortos, NULL)){
        printf("\n[BALCÃO] Ocorreu um erro ao criar a thread removerMortos!\n");
        unlink(BALCAO_FIFO);
        unlink(BALCAO_FIFO_MED);
        unlink(BALCAO_FIFO_CLI);
        return 0;
    }
    if(pthread_create(&thread_id, NULL, consolaAdministrador, NULL)){
        printf("\n[BALCÃO] Ocorreu um erro ao criar a thread consolaAdministrador!\n");
        unlink(BALCAO_FIFO);
        unlink(BALCAO_FIFO_MED);
        unlink(BALCAO_FIFO_CLI);
        return 0;
    }
    pthread_join(thread_id, NULL);

    unlink(BALCAO_FIFO);
    unlink(BALCAO_FIFO_MED);
    unlink(BALCAO_FIFO_CLI);

    return 0;
}