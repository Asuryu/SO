#include "dict.h"
#include <signal.h>

#define NPALAVRAS 7 /* Número de palavras conhecidas */

char* dicionario[NPALAVRAS][2] = {
	{"memory", "memória"},
	{"computer","computador"},
	{"close","fechar"},
	{"open","abrir"},
	{"read","ler"},
	{"write","escrever"},
	{"file","ficheiro"}};

int	s_fifo_fd, c_fifo_fd;

/* Esta função vai atender o sinal SIGINT para terminar o servidor */
void	trataSig(int i)
{
	(void) i;
	fprintf(stderr, "\nServidor de dicionário a terminar "
									"(interrompido via teclado)\n\n");
	close(s_fifo_fd);
	unlink(SERVER_FIFO);
	exit(EXIT_SUCCESS);
}

int	main(void)
{
	pergunta_t	perg;
	resposta_t	resp;

	int	res;
	int	i;
	char	c_fifo_fname[50];
	printf("\nServidor de dicionário");
	if (signal(SIGINT, trataSig) == SIG_ERR)
	{
		perror("\nNão foi possível configurar o sinal SIGINT\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "\nSinal SIGINT configurado");
	res = mkfifo(SERVER_FIFO, 0777);
	if (res == -1)
	{
		perror("\nmkfifo do FIFO do servidor deu erro");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "\nFIFO servidor criado");

	/* prepara FIFO do servidor */
	/* abertura read+write -> evita o comportamento de ficar	*/
	/* bloqueado no open. a execução prossegue e as						*/
	/* operações read/write (neste caso apenas READ)					*/
	/* continuam bloqueantes (mais fácil de gerir)						*/
	s_fifo_fd = open(SERVER_FIFO, O_RDWR);
	if (s_fifo_fd == -1)
	{
		perror("\nErro ao abrir o FIFO do servidor (RDWR/blocking)");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "\nFIFO aberto para READ (+WRITE) bloqueante");
	
	/* ciclo principal: read pedido -> write resposta -> repete */
	while (1) /* sai via SIGINT */
	{
		/* ---- OBTEM PERGUNTA ---- */
		res = read(s_fifo_fd, &perg, sizeof(perg));
		if (res < (int) sizeof(perg))
		{
			fprintf(stderr, "\nRecebida pergunta incompleta "
											"[bytes lidos: %d]", res);
			continue; /* não responde a cliente (qual cliente?) */
		}
		fprintf(stderr, "\nRecebido [%s]", perg.palavra);

		/* ---- PROCURA TRADUÇÃO ---- */
		strcpy(resp.palavra, "DESCONHECIDO"); /* caso não encontre */
		for (i = 0; i < (int)NPALAVRAS; i++)
		{
			if (!strcasecmp(perg.palavra, dicionario[i][0]))
			{
				strcpy(resp.palavra, dicionario[i][1]);
				break;
			}
		}
		fprintf(stderr, "\nResposta = [%s]", resp.palavra);

		/* ---- OBTÉM FILENAME DO FIFO PARA A RESPOSTA ---- */
		sprintf(c_fifo_fname, CLIENT_FIFO, perg.pid_cliente);

		/* ---- ABRE O FIFO do cliente p/ write ---- */
		c_fifo_fd = open(c_fifo_fname, O_WRONLY);
		if (c_fifo_fd == -1)
			perror("\nErro no open - Ninguém quis a resposta");
		else
		{
			fprintf(stderr, "\nFIFO cliente aberto para WRITE");

			/* ---- ENVIA RESPOSTA ---- */
			res = write(c_fifo_fd, &resp, sizeof(resp));
			if (res == sizeof(resp))
				fprintf(stderr, "\nescreveu a resposta");
			else
				perror("\nerro a escrever a resposta");

			close(c_fifo_fd); /* FECHA LOGO O FIFO DO CLIENTE! */
			fprintf(stderr, "\nFIFO cliente fechado");
		}
	} /* fim do ciclo principal do servidor */

	/* em princípio não chega a este ponto - sai via SIGINT */
	close(s_fifo_fd);
	unlink(SERVER_FIFO);
	return (0);
}
