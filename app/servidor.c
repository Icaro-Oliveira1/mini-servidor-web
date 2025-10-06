#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include "tslog.h"

// --- Configurações ---
#define PORTA 8080
#define MAX_CONEXOES_PENDENTES 20 // Limite configurável
#define BUFFER_SIZE 4096
#define WWW_PATH "./www"
#define PID_FILE "server.pid"

// --- Globais ---
ts_logger_t *g_logger;
int g_socket_servidor;
volatile long g_requests_servidos = 0;
pthread_mutex_t g_stats_mutex;

// --- Estruturas ---
typedef struct {
    int socket_cliente;
    char *ip_cliente;
} conexao_cliente_t;

// --- Protótipos ---
void handle_signal(int sig);

// --- Implementação ---

void enviar_resposta(int socket_cliente, const char *header, const char *content_type, const char *body, long body_len) {
    char http_response[BUFFER_SIZE];
    int header_len = sprintf(http_response, "%s\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n",
                             header, content_type, body_len);
    write(socket_cliente, http_response, header_len);
    write(socket_cliente, body, body_len);
}

void *handle_connection(void *arg) {
    conexao_cliente_t *conexao = (conexao_cliente_t *)arg;
    int socket_cliente = conexao->socket_cliente;
    char *ip_cliente = conexao->ip_cliente;
    char buffer[BUFFER_SIZE] = {0};

    read(socket_cliente, buffer, BUFFER_SIZE - 1);
    char *primeira_linha = strtok(buffer, "\r\n");
    if (primeira_linha == NULL) primeira_linha = "";

    tslog_log(g_logger, "INFO", "Requisição de %s: \"%s\"", ip_cliente, primeira_linha);

    char metodo[16], uri[256];
    sscanf(primeira_linha, "%s %s", metodo, uri);

    if (strcmp(metodo, "GET") == 0) {
        char caminho_arquivo[512];
        if (strcmp(uri, "/") == 0) sprintf(caminho_arquivo, "%s/index.html", WWW_PATH);
        else sprintf(caminho_arquivo, "%s%s", WWW_PATH, uri);

        FILE *file = fopen(caminho_arquivo, "rb");
        if (file == NULL) {
            tslog_log(g_logger, "WARN", "404 Not Found: %s", caminho_arquivo);
            char body[] = "<html><body><h1>404 Not Found</h1></body></html>";
            enviar_resposta(socket_cliente, "HTTP/1.0 404 Not Found", "text/html", body, strlen(body));
        } else {
            fseek(file, 0, SEEK_END);
            long tamanho = ftell(file);
            fseek(file, 0, SEEK_SET);
            char *conteudo = malloc(tamanho);
            fread(conteudo, 1, tamanho, file);
            fclose(file);
            
            // Assumindo text/html por simplicidade
            enviar_resposta(socket_cliente, "HTTP/1.0 200 OK", "text/html", conteudo, tamanho);
            free(conteudo);
        }
    } else {
        tslog_log(g_logger, "WARN", "501 Not Implemented: %s", metodo);
        char body[] = "<html><body><h1>501 Not Implemented</h1></body></html>";
        enviar_resposta(socket_cliente, "HTTP/1.0 501 Not Implemented", "text/html", body, strlen(body));
    }

    // Atualiza estatísticas de forma segura
    pthread_mutex_lock(&g_stats_mutex);
    g_requests_servidos++;
    pthread_mutex_unlock(&g_stats_mutex);
    
    close(socket_cliente);
    free(ip_cliente);
    free(conexao);
    return NULL;
}

void start_server() {
    // Configura o tratamento de sinais para desligamento gracioso
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    struct sockaddr_in endereco_servidor;
    g_socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(g_socket_servidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_addr.s_addr = INADDR_ANY;
    endereco_servidor.sin_port = htons(PORTA);

    if (bind(g_socket_servidor, (struct sockaddr *)&endereco_servidor, sizeof(endereco_servidor)) < 0) {
        tslog_log(g_logger, "FATAL", "Falha no bind na porta %d.", PORTA); exit(1);
    }
    if (listen(g_socket_servidor, MAX_CONEXOES_PENDENTES) < 0) {
        tslog_log(g_logger, "FATAL", "Falha ao escutar."); exit(1);
    }
    tslog_log(g_logger, "SYSTEM", "Servidor escutando na porta %d.", PORTA);

    while (1) {
        struct sockaddr_in endereco_cliente;
        socklen_t tamanho_endereco = sizeof(endereco_cliente);
        int socket_cliente = accept(g_socket_servidor, (struct sockaddr *)&endereco_cliente, &tamanho_endereco);
        if (socket_cliente < 0) continue;

        conexao_cliente_t *conexao = malloc(sizeof(conexao_cliente_t));
        conexao->socket_cliente = socket_cliente;
        conexao->ip_cliente = strdup(inet_ntoa(endereco_cliente.sin_addr));
        
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_connection, (void *)conexao);
        pthread_detach(thread_id);
    }
}

// Função para rodar o servidor em background
void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Processo pai termina

    if (setsid() < 0) exit(EXIT_FAILURE); // Cria nova sessão

    // Salva o PID do processo filho (daemon)
    FILE *pid_file = fopen(PID_FILE, "w");
    if (pid_file) {
        fprintf(pid_file, "%d", getpid());
        fclose(pid_file);
    }
    
    // Redireciona stdin, stdout, stderr para /dev/null
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    start_server();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s [start|stop|status]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "start") == 0) {
        g_logger = tslog_init("server.log");
        pthread_mutex_init(&g_stats_mutex, NULL);
        tslog_log(g_logger, "SYSTEM", "Iniciando servidor em modo daemon...");
        printf("Iniciando servidor...\n");
        daemonize();
    } else if (strcmp(argv[1], "stop") == 0) {
        FILE *pid_file = fopen(PID_FILE, "r");
        if (!pid_file) {
            printf("Servidor não parece estar rodando (PID file não encontrado).\n");
            return 1;
        }
        pid_t pid;
        fscanf(pid_file, "%d", &pid);
        fclose(pid_file);
        
        if (kill(pid, SIGTERM) == 0) {
            remove(PID_FILE);
            printf("Sinal de parada enviado para o servidor (PID: %d).\n", pid);
        } else {
            printf("Falha ao enviar sinal de parada (PID: %d). O processo existe?\n", pid);
        }
    } else if (strcmp(argv[1], "status") == 0) {
        FILE *pid_file = fopen(PID_FILE, "r");
        if (pid_file) {
            pid_t pid;
            fscanf(pid_file, "%d", &pid);
            fclose(pid_file);
            printf("Servidor está rodando com PID: %d\n", pid);
        } else {
            printf("Servidor está parado.\n");
        }
    } else {
        fprintf(stderr, "Comando desconhecido: %s\n", argv[1]);
        return 1;
    }

    return 0;
}

// Handler para desligamento limpo
void handle_signal(int sig) {
    tslog_log(g_logger, "SYSTEM", "Sinal de parada recebido. Desligando...");
    tslog_log(g_logger, "STATS", "Total de requisições servidas: %ld", g_requests_servidos);
    close(g_socket_servidor);
    tslog_destroy(g_logger);
    pthread_mutex_destroy(&g_stats_mutex);
    remove(PID_FILE);
    exit(0);
}