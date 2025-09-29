#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include "tslog.h"

ts_logger_t* tslog_init(const char *filename) {
    // Aloca memória para a estrutura do logger
    ts_logger_t *logger = malloc(sizeof(ts_logger_t));
    if (logger == NULL) {
        perror("tslog_init: malloc");
        return NULL;
    }

    // Abre o arquivo no modo "append" (adicionar ao final do arquivo)
    logger->file = fopen(filename, "a");
    if (logger->file == NULL) {
        perror("tslog_init: fopen");
        free(logger); // Libera a memória alocada se o arquivo não puder ser aberto
        return NULL;
    }

    // Inicializa o mutex
    if (pthread_mutex_init(&logger->mutex, NULL) != 0) {
        perror("tslog_init: pthread_mutex_init");
        fclose(logger->file);
        free(logger);
        return NULL;
    }

    return logger;
}

void tslog_log(ts_logger_t *logger, const char *level, const char *format, ...) {
    if (logger == NULL || logger->file == NULL) {
        return; // Segurança: não faz nada se o logger não for válido
    }

    // --- Início da Seção Crítica ---
    // Trava o mutex. Apenas uma thread pode executar este trecho de cada vez.
    pthread_mutex_lock(&logger->mutex);

    // 1. Obtém a data e hora atual
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_buf[80];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", t);

    // 2. Imprime o cabeçalho do log (timestamp, nível, ID da thread)
    fprintf(logger->file, "[%s] [%s] [ThreadID: %lu] ", time_buf, level, (unsigned long)pthread_self());

    // 3. Imprime a mensagem do usuário
    va_list args;
    va_start(args, format);
    vfprintf(logger->file, format, args);
    va_end(args);

    // 4. Adiciona uma nova linha e força a escrita no disco
    fprintf(logger->file, "\n");
    fflush(logger->file);

    // Libera o mutex, permitindo que outra thread entre na seção crítica.
    pthread_mutex_unlock(&logger->mutex);
    // --- Fim da Seção Crítica ---
}

void tslog_destroy(ts_logger_t *logger) {
    if (logger != NULL) {
        if (logger->file != NULL) {
            fclose(logger->file);
        }
        pthread_mutex_destroy(&logger->mutex);
        free(logger);
    }
}