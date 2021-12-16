#ifndef DICT_H
#define DICT_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

/* nome do FIFO do servidor */
#define SERVER_FIFO "/tmp/dict_fifo"

/* nome do FIFO cada cliente. P %d será substituído pelo PID com sprintf */
#define CLIENT_FIFO "/tmp/resp_%d_fifo"

/* tamanho máximo de cada palavra */
#define TAM_MAX 50

/* estrutura de mensagem correspondente a um pedido cliente -> servidor*/
typedef struct {
	pid_t	pid_cliente;
	char	palavra[TAM_MAX];
} pergunta_t;

/* estrutura da mensagem correspondente a uma resposta do servidor */
typedef struct {
	char	palavra[TAM_MAX];
} resposta_t;

#endif
