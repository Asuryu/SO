BALCAO = balcao
CLIENTE = cliente
MEDICO = medico
C = gcc

CFLAGS = -Wall -Werror -Wextra

all:
	@$(C) ./$(BALCAO)/$(BALCAO).c -o ./$(BALCAO)/$(BALCAO) $(CFLAGS) -pthread
	@$(C) ./$(CLIENTE)/$(CLIENTE).c -o ./$(CLIENTE)/$(CLIENTE) $(CFLAGS) 
	@$(C) ./$(MEDICO)/$(MEDICO).c -o ./$(MEDICO)/$(MEDICO) $(CFLAGS) -pthread
balcao:
	@$(C) ./$(BALCAO)/$(BALCAO).c -o ./$(BALCAO)/$(BALCAO) $(CFLAGS) -pthread
cliente:
	@$(C) ./$(CLIENTE)/$(CLIENTE).c -o ./$(CLIENTE)/$(CLIENTE) $(CFLAGS)
medico:
	@$(C) ./$(MEDICO)/$(MEDICO).c -o ./$(MEDICO)/$(MEDICO) $(CFLAGS) -pthread
clean:
	@rm ./$(BALCAO)/$(BALCAO) ./$(CLIENTE)/$(CLIENTE) ./$(MEDICO)/$(MEDICO)
