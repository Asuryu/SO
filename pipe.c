#include <sys/stat.h>

int main(){
char mensagem[100];
strcpy(mensagem, "ola do cliente");
mkfifo(SERVER_FIFO, 0666); (Cria o fifo servidor)
int fd = open (SERVER_FIFO, O_RDONLY); (Abre o ficheiro para leitura)
if(fd== -1) printf("Erro");
int size = read (fd.&mensagem, sizeof(mensagem)); (Lê o ficheiro)
if (size > 0)
printf("Tamanho [%d] mensagem [%s]", size, mensagem); (Verifica se corre bem)
close(fd);
unlink(SERVER_FIFO); (Apaga o fifo)
};

if(mkfifo(SERVER_FIFO, 0666) == -1){
if (ERRNO == exists)
printf("Fifo já existe");
printf("Erro ao abrir o fifo");
return 1;
