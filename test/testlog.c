#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "tslog.h"

#define NUM_THREADS 5
#define NUM_MESSAGES 10

// O logger será uma variável global para que todas as threads possam acessá-lo
ts_logger_t *g_logger;

// Função que cada thread irá executar
void *worker(void *arg) {
    long thread_num = (long)arg;

    for (int i = 0; i < NUM_MESSAGES; i++) {
        tslog_log(g_logger, "INFO", "Mensagem %d do trabalhador %ld.", i + 1, thread_num);
        // Pequena pausa para dar chance a outras threads de tentarem escrever também
        usleep(10000); 
    }

    tslog_log(g_logger, "DEBUG", "Trabalhador %ld terminou.", thread_num);
    return NULL;
}

int main() {
    printf("Iniciando teste da biblioteca de logging...\n");

    // Inicializa o logger para escrever no arquivo "test.log"
    g_logger = tslog_init("test.log");
    if (g_logger == NULL) {
        fprintf(stderr, "Falha ao inicializar o logger.\n");
        return 1;
    }

    tslog_log(g_logger, "SYSTEM", "Logger inicializado. Criando %d threads...", NUM_THREADS);

    pthread_t threads[NUM_THREADS];
    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, (void *)(i + 1));
    }

    // Espera que todas as threads completem seu trabalho
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    tslog_log(g_logger, "SYSTEM", "Teste concluído. Desligando o logger.");
    
    // Libera os recursos do logger
    tslog_destroy(g_logger);

    printf("Teste finalizado. Verifique o conteúdo do arquivo 'test.log'.\n");
    return 0;
}