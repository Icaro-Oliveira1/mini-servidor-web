#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "tslog.h" // Inclui a sua biblioteca de logging!

#define PORTA 8080
#define MAX_CONEXOES_PENDENTES 10 // Requisito: Fila de conexões configurável
#define BUFFER_SIZE 2048
#define WWW_PATH "./www" // Pasta onde os arquivos do site ficarão

ts_logger_t *g_logger; // Logger global para ser acessado por todas as threads

// Estrutura para passar dados para a thread de atendimento
typedef struct {
    int socket_cliente;
    char *ip_cliente;
} conexao_cliente_t;

// Função para enviar uma resposta HTTP
void enviar_resposta(int socket_cliente, const char *header, const char *content_type, const char *body) {
    char http_response[BUFFER_SIZE];
    sprintf(http_response, "%s\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n%s",
            header, content_type, strlen(body), body);
    write(socket_cliente, http_response, strlen(http_response));
}

// Função executada por cada thread para atender um cliente
void *handle_connection(void *arg) {
    conexao_cliente_t *conexao = (conexao_cliente_t *)arg;
    int socket_cliente = conexao->socket_cliente;
    char *ip_cliente = conexao->ip_cliente;
    char buffer[BUFFER_SIZE] = {0};

    // Lê a requisição do cliente
    read(socket_cliente, buffer, BUFFER_SIZE - 1);

    tslog_log(g_logger, "INFO", "Requisição recebida de %s: \"%s\"", ip_cliente, strtok(buffer, "\r\n"));

    // --- Parser HTTP Mínimo ---
    char metodo[16], uri[256];
    sscanf(buffer, "%s %s", metodo, uri);

    if (strcmp(metodo, "GET") == 0) {
        char caminho_arquivo[512];
        if (strcmp(uri, "/") == 0) {
            sprintf(caminho_arquivo, "%s/index.html", WWW_PATH);
        } else {
            sprintf(caminho_arquivo, "%s%s", WWW_PATH, uri);
        }

        FILE *file = fopen(caminho_arquivo, "r");
        if (file == NULL) {
            // Arquivo não encontrado
            tslog_log(g_logger, "WARN", "Arquivo não encontrado (%s) para o cliente %s", caminho_arquivo, ip_cliente);
            char *body = "<html><body><h1>404 Not Found</h1></body></html>";
            enviar_resposta(socket_cliente, "HTTP/1.0 404 Not Found", "text/html", body);
        } else {
            // Arquivo encontrado, envia o conteúdo
            tslog_log(g_logger, "INFO", "Servindo arquivo %s para o cliente %s", caminho_arquivo, ip_cliente);
            
            fseek(file, 0, SEEK_END);
            long tamanho_arquivo = ftell(file);
            fseek(file, 0, SEEK_SET);

            char *conteudo_arquivo = malloc(tamanho_arquivo + 1);
            fread(conteudo_arquivo, 1, tamanho_arquivo, file);
            conteudo_arquivo[tamanho_arquivo] = '\0';
            fclose(file);
            
            enviar_resposta(socket_cliente, "HTTP/1.0 200 OK", "text/html", conteudo_arquivo);
            free(conteudo_arquivo);
        }
    } else {
        // Método não suportado
        tslog_log(g_logger, "WARN", "Método não suportado (%s) do cliente %s", metodo, ip_cliente);
        char *body = "<html><body><h1>501 Not Implemented</h1></body></html>";
        enviar_resposta(socket_cliente, "HTTP/1.0 501 Not Implemented", "text/html", body);
    }

    close(socket_cliente);
    free(ip_cliente);
    free(conexao);
    return NULL;
}

int main(int argc, char *argv[]) {
    // Inicializa o logger
    g_logger = tslog_init("server.log");
    if (g_logger == NULL) {
        fprintf(stderr, "Falha ao inicializar o logger.\n");
        exit(1);
    }

    int socket_servidor;
    struct sockaddr_in endereco_servidor;

    tslog_log(g_logger, "SYSTEM", "Iniciando servidor...");

    socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor < 0) {
        tslog_log(g_logger, "FATAL", "Falha ao criar o socket.");
        exit(1);
    }
    
    int opt = 1;
    setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_addr.s_addr = INADDR_ANY;
    endereco_servidor.sin_port = htons(PORTA);

    if (bind(socket_servidor, (struct sockaddr *)&endereco_servidor, sizeof(endereco_servidor)) < 0) {
        tslog_log(g_logger, "FATAL", "Falha ao fazer o bind na porta %d.", PORTA);
        exit(1);
    }

    if (listen(socket_servidor, MAX_CONEXOES_PENDENTES) < 0) {
        tslog_log(g_logger, "FATAL", "Falha ao escutar.");
        exit(1);
    }
    
    tslog_log(g_logger, "SYSTEM", "Servidor escutando na porta %d.", PORTA);
    printf("Servidor escutando na porta %d. Acesse http://localhost:%d\n", PORTA, PORTA);

    while (1) {
        struct sockaddr_in endereco_cliente;
        socklen_t tamanho_endereco = sizeof(endereco_cliente);
        int socket_cliente = accept(socket_servidor, (struct sockaddr *)&endereco_cliente, &tamanho_endereco);

        if (socket_cliente < 0) {
            tslog_log(g_logger, "ERROR", "Falha ao aceitar conexão.");
            continue;
        }

        // Prepara os dados para a nova thread
        conexao_cliente_t *conexao = malloc(sizeof(conexao_cliente_t));
        conexao->socket_cliente = socket_cliente;
        conexao->ip_cliente = strdup(inet_ntoa(endereco_cliente.sin_addr));
        
        // Cria a thread para atender o cliente
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_connection, (void *)conexao);
        pthread_detach(thread_id); // Permite que a thread libere seus recursos ao terminar
    }

    close(socket_servidor);
    tslog_destroy(g_logger);
    return 0;
}