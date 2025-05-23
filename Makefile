CC = gcc
CFLAGS = -Wall -Wextra -pedantic -fPIC -g -O0 -pthread
LDFLAGS = -lpthread
CLIENT_SRC = app-cliente.c
SERVER_SRC = servidor-sock.c
PROXY_SRC = proxy-sock.c
KEYS_SRC = claves.c
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o)
PROXY_OBJ = $(PROXY_SRC:.c=.o)
KEYS_OBJ = $(KEYS_SRC:.c=.o)
CLIENT1_SRC = app-cliente-1.c
CLIENT1_OBJ = $(CLIENT1_SRC:.c=.o)
CLIENT1_EXEC = cliente1
CLIENT2_SRC = app-cliente-2.c
CLIENT2_OBJ = $(CLIENT2_SRC:.c=.o)
CLIENT2_EXEC = cliente2
CLIENT3_SRC = app-cliente-3.c
CLIENT3_OBJ = $(CLIENT3_SRC:.c=.o)
CLIENT3_EXEC = cliente3
LIB_NAME = libclaves.so
SERVER_EXEC = servidor
CLIENT_EXEC = cliente

all: $(SERVER_EXEC) $(CLIENT_EXEC)	$(CLIENT1_EXEC)	$(CLIENT2_EXEC) $(CLIENT3_EXEC)

$(LIB_NAME): $(PROXY_OBJ)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

$(SERVER_EXEC): $(SERVER_OBJ) $(KEYS_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(CLIENT_EXEC): $(CLIENT_OBJ) $(LIB_NAME)
	$(CC) -o $@ $< -L. -lclaves -Wl,-rpath=.

$(CLIENT1_EXEC): $(CLIENT1_OBJ) $(LIB_NAME)
	$(CC) -o $@ $< -L. -lclaves -Wl,-rpath=.

$(CLIENT2_EXEC): $(CLIENT2_OBJ) $(LIB_NAME)
	$(CC) -o $@ $< -L. -lclaves -Wl,-rpath=.

$(CLIENT3_EXEC): $(CLIENT3_OBJ) $(LIB_NAME)
	$(CC) -o $@ $< -L. -lclaves -Wl,-rpath=.



%.o: %.c claves.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC) $(CLIENT1_EXEC)	$(CLIENT2_EXEC) $(CLIENT3_EXEC) $(LIB_NAME) *.o

.PHONY: all clean