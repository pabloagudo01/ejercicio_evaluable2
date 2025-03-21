CC = gcc
CFLAGS = -Wall -Wextra -pedantic -fPIC
LDFLAGS = -lpthread
CLIENT_SRC = app-cliente.c
SERVER_SRC = servidor-sock.c
PROXY_SRC = proxy-sock.c
KEYS_SRC = claves.c
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o)
PROXY_OBJ = $(PROXY_SRC:.c=.o)
KEYS_OBJ = $(KEYS_SRC:.c=.o)
LIB_NAME = libclaves.so
SERVER_EXEC = servidor
CLIENT_EXEC = cliente

all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(LIB_NAME): $(PROXY_OBJ)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

$(SERVER_EXEC): $(SERVER_OBJ) $(KEYS_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(CLIENT_EXEC): $(CLIENT_OBJ) $(LIB_NAME)
	$(CC) -o $@ $(CLIENT_OBJ) -L. -lclaves

%.o: %.c claves.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC) $(LIB_NAME) *.o

.PHONY: all clean