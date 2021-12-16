SERVER = dserv

CLIENT = dcli

C = gcc

CFLAGS = -Wall -Werror -Wextra

all:
	@$(C) $(CFLAGS) $(CLIENT).c -o $(CLIENT)
	@$(C) $(CFLAGS) $(SERVER).c -o $(SERVER)
clean:
	@rm $(CLIENT) $(SERVER)
