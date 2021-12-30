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

char FIFO_FINAL[MAX];
char **args;
balcao b;

int onlyBalcao(){
    int fd_balcao = open(BALCAO_FIFO, O_RDONLY | O_NONBLOCK);
    close(fd_balcao);
    if(fd_balcao == -1){
        return 1;
    }
    return 0;
}

void *aceitarMedicos(void *vargp){

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

void *aceitarClientes(void *vargp){

    cliente c;

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
                // PEDIR AO CLASSIFICADOR PARA CLASSIFICAR OS SINTOMAS
                // ENVIAR AO CLIENTE O RESULTADO
                // IDEIA: Criar função para enviar uma mensagem para o classificador
                
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

                b.clienteEspera[b.nClientesEspera] = c;
                b.nClientesEspera++;
            }
        }
    } while (1);
}


void ListaEspera(int signum){
    for(int i = 0; i < b.nClientesEspera; i++){
        printf("Cliente %s em lista de espera %d", b.clienteEspera->nome, b.clientes->pid);
    };
};

void *TemporizadorAlarme(void *vargp){
    struct sigaction sa;
    sa.sa_handler = ListaEspera;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGALRM, &sa, NULL);
  
    while(1){
        alarm(10);
        pause();
    }
};

void *consolaAdministrador(void *vargp){

    char sintomas[MAX];
    char sintomasFinais[MAX];
    char analise[MAX];
    
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
    while (1){ // Ciclo para pedir os sintomas ao utilizador
        strcpy(analise,"");
        strcpy(sintomas,"");
        strcpy(sintomasFinais,"");

        // Get sintomas do utilizador
        // Prevent user from inputting blank lines
        printf("\nIntroduza um sintoma (debug): ");
        do{
            fgets(sintomas, MAX, stdin);
            if(strcmp(sintomas, "\n") == 0){
                printf("Introduza um sintoma (debug): ");
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
                    printf("Utente [%d] %s com o sintoma %s\n", b.clientes[i].pid, b.clientes[i].nome, b.clientes[i].sintomas);
                }
                if(b.nClientesEspera > 0){
                    printf("\n[BALCÃO] A listar todos os utentes em espera (%d):\n", b.nClientesEspera);
                    for(int i=0; i < b.nClientesEspera; i++)
                    {
                        printf("Utente [%d] %s com o sintoma %s\n", b.clienteEspera[i].pid, b.clienteEspera[i].nome, b.clienteEspera[i].sintomas);
                    }
                }
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
                int pid = atoi(args[1]);
                if(pid != 0){
                    int j = 0;
                    for(int i=0; i < b.nClientesAtivos; i++){
                        if(b.clientes[i].pid == pid){
                            j = i;
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 0) printf("\n[BALCÃO] O utente com o PID %d não existe", pid);
                    else{
                        printf("\n[BALCÃO] O utente com o PID %d foi removido", pid);
                        b.clientes[j] = b.clientes[b.nClientesAtivos - 1];
                        b.nClientesAtivos--;
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
                        if(b.medicos[i].pid == pid){
                            j = i;
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 0) printf("\n[BALCÃO] O médico com o PID %d não existe", pid);
                    else{
                        printf("\n[BALCÃO] O médico com o PID %d foi removido", pid);
                        b.medicos[j] = b.medicos[b.nMedicosAtivos - 1];
                        b.nMedicosAtivos--;
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
                    printf("Estão %d utentes em lista de espera", b.nClientesEspera); 
                    pthread_t thread_id;
                    pthread_create(&thread_id, NULL, TemporizadorAlarme, NULL);
                    pthread_join(thread_id, NULL);
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
        else {
            strcat(sintomasFinais, "\n");
            write(b.unpipeBC[1], sintomasFinais, strlen(sintomasFinais)); // Escrever para o pipe Balcão -> Classificador o conteúdo da variável sintomas
            int tmp = read(b.unpipeCB[0], analise, MAX); // Ler para a variável análise o conteúdo existente no pipe Classificador -> Balcão
            analise[tmp-1]= '\0';
            printf("O classificador retornou: %s", analise);
            fflush(stdout);
            fflush(stdin);
        }
        free(args);
    }
    write(b.unpipeBC[1], "#fim\n", strlen("#fim\n"));
    printf("\n[BALCÃO] A encerrar o balcão...\n");
    wait(NULL); // Esperar que o processo filho termine
    close(b.unpipeBC[1]); // Fecha o write do pipe Balcão -> Classificador
    close(b.unpipeCB[0]); // Fecha o read do pipe Classificador -> Balcão 
    free(args);
    return NULL;
}

int main(int argc, char *argv[]){

    printf("\033[2J\033[1;1H");
    printf("     __ __   __          __ __ \n");
    printf("|\\/||_ |  \\|/   /\\ |    (_ /  \\ \n");
    printf("|  ||__|__/|\\__/--\\|__  __)\\__/\n\n");
    printf("\n[BALCÃO]\nBem vindo ao MEDICALso, Administrador");
    fflush(stdout);
    
    pthread_t thread_id;

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