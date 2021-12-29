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


#define BALCAO_FIFO "../MEDICALso"
#define BALCAO_FIFO_MED "../MEDICALsoMED"
#define BALCAO_FIFO_CLI "../MEDICALsoCLI"
#define CLIENTE_FIFO "../CLIENTE[%d]"
#define MEDICO_FIFO "../MEDICO[%d]"

char FIFO_FINAL[MAX];

int onlyBalcao(){
    int fd_balcao = open(BALCAO_FIFO, O_RDONLY | O_NONBLOCK);
    close(fd_balcao);
    if(fd_balcao == -1){
        return 1;
    }
    return 0;
}

void *aceitarMedicos(void *vargp){

    balcao b = *((balcao*)vargp);
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

    balcao b = *((balcao*)vargp);
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
            }
        }
    } while (1);
}

void *consolaAdministrador(void *vargp){

    char sintomas[MAX];
    char analise[MAX];
    balcao b = *((balcao*)vargp); // Struct do tipo balcão
    
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
        printf("\nIndique os seus sintomas (debug): ");
        fgets(sintomas, sizeof(sintomas), stdin);
        sintomas[strlen(sintomas) - 1] = '\0';
        strcat(sintomas, "\n");

        if(!strcmp(sintomas, "#fim\n")) break;
        else if(!strcmp(sintomas, "utentes\n")){
            printf("A listar todos os utentes...");
            for(int i=0; i < b.nClientesAtivos; i++)
            {
                printf("\nUtente [%s] %d com o sintoma %s\n", b.clientes[i].nome, b.clientes[i].pid, b.clientes[i].sintomas);
            }
        }
            
        else if(!strcmp(sintomas, "especialistas\n")) printf("A listar todos os especialistas...");
        else if(!strncmp(sintomas, "delut", strlen("delut"))) printf("Utilizador XYZ removido");
        else if(!strncmp(sintomas, "delesp", strlen("delesp"))) printf("Especialista XYZ removido");
        else if(!strncmp(sintomas, "freq", strlen("freq"))) printf("A apresentar a ocupação das filas de X em X segundos...");
        else if(!strcmp(sintomas, "encerra\n")) break;
        else {
            write(b.unpipeBC[1], sintomas, strlen(sintomas)); // Escrever para o pipe Balcão -> Classificador o conteúdo da variável sintomas
            int tmp = read(b.unpipeCB[0], analise, MAX); // Ler para a variável análise o conteúdo existente no pipe Classificador -> Balcão
            analise[tmp-1]= '\0';
            printf("O classificador retornou: %s", analise);
            fflush(stdout);
            fflush(stdin);
        }
    }
    write(b.unpipeBC[1], "#fim\n", strlen("#fim\n"));
    wait(NULL); // Esperar que o processo filho termine
    close(b.unpipeBC[1]); // Fecha o write do pipe Balcão -> Classificador
    close(b.unpipeCB[0]); // Fecha o read do pipe Classificador -> Balcão
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
    balcao b;

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

    if(pthread_create(&thread_id, NULL, aceitarMedicos, &b)){
        printf("\n[BALCÃO] Ocorreu um erro ao criar a thread aceitarMedicos!\n");
        unlink(BALCAO_FIFO);
        unlink(BALCAO_FIFO_MED);
        unlink(BALCAO_FIFO_CLI);
        return 0;
    }
    if(pthread_create(&thread_id, NULL, aceitarClientes, &b)){
        printf("\n[BALCÃO] Ocorreu um erro ao criar a thread aceitarClientes!\n");
        unlink(BALCAO_FIFO);
        unlink(BALCAO_FIFO_MED);
        unlink(BALCAO_FIFO_CLI);
        return 0;
    }
    if(pthread_create(&thread_id, NULL, consolaAdministrador, &b)){
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