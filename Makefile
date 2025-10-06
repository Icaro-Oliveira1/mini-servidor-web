# Alvo principal padrão (o que é compilado quando se digita 'make')
all: servidor

# --- Alvos ---
TARGET_SERVER = servidor
TARGET_TEST = test_log

# --- Compilador e Flags ---
CC = gcc
VPATH = lib:app:test
CFLAGS = -Ilib -Wall -pthread

# --- Fontes ---
SERVER_SRCS = servidor.c tslog.c
TEST_SRCS = test_log.c tslog.c

# --- Objetos ---
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

# --- Regras de Build ---

# Regra para construir o servidor
servidor: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(SERVER_OBJS)

# Regra para construir o programa de teste
test: $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET_TEST) $(TEST_OBJS)

# Regra genérica para compilar .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Regras de Limpeza ---
clean:
	rm -f $(SERVER_OBJS) $(TEST_OBJS) $(TARGET_SERVER) $(TARGET_TEST) *.log *.pid