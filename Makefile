# Nome do programa de teste final
TARGET = test_log

# Compilador
CC = gcc

# Flags de compilação
# -Ilib: Diz ao compilador para procurar por arquivos de header (#include) na pasta 'lib'
# -Wall: Habilita todos os avisos (warnings)
# -pthread: Necessário para linkar a biblioteca de threads
CFLAGS = -Ilib -Wall -pthread

# Arquivos fonte da biblioteca
LIB_SRCS = lib/tslog.c

# Arquivos fonte do teste
TEST_SRCS = test/testlog.c

# Converte a lista de fontes (.c) para uma lista de objetos (.o)
OBJS = $(LIB_SRCS:.c=.o) $(TEST_SRCS:.c=.o)

# Regra principal: o que fazer quando digitar 'make'
all: $(TARGET)

# Regra para linkar o programa final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regra de compilação genérica: como transformar um .c em um .o
# Funciona para arquivos em qualquer subdiretório
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os arquivos gerados
clean:
	rm -f $(OBJS) $(TARGET) test.log