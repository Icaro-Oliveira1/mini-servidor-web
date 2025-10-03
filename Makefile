# Alvo principal agora é o servidor
TARGET = servidor

# Compilador
CC = gcc

# VPATH para encontrar os fontes
VPATH = lib:app

# Flags de compilação (-Ilib para o header, -Iapp se necessário, -pthread para threads)
CFLAGS = -Ilib -Wall -pthread

# Fontes da biblioteca e da aplicação
LIB_SRCS = tslog.c
APP_SRCS = servidor.c

# Lista final de fontes
SRCS = $(LIB_SRCS) $(APP_SRCS)

# Lista de objetos
OBJS = $(SRCS:.c=.o)

# Regra principal
all: $(TARGET)

# Regra para linkar o executável final do servidor
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regra genérica para compilar .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar tudo
clean:
	rm -f $(OBJS) $(TARGET) *.log