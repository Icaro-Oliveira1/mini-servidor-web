#ifndef TSLOG_H
#define TSLOG_H

#include <pthread.h> // Incluímos aqui para que o tipo pthread_mutex_t seja conhecido
#include <stdio.h>
/**
 * @brief Estrutura interna do logger.
 * Não deve ser manipulada diretamente pelo usuário da biblioteca.
 */
typedef struct {
    FILE *file;
    pthread_mutex_t mutex;
} ts_logger_t;

/**
 * @brief Inicia a biblioteca de logging.
 * Abre o arquivo de log especificado e inicializa o mutex.
 * * @param filename O caminho para o arquivo de log.
 * @return Um ponteiro para a estrutura do logger em caso de sucesso, NULL em caso de erro.
 */
ts_logger_t* tslog_init(const char *filename);

/**
 * @brief Escreve uma mensagem de log de forma segura (thread-safe).
 * * @param logger O ponteiro para o logger retornado por tslog_init.
 * @param level O nível da mensagem (ex: "INFO", "ERROR", "DEBUG").
 * @param format A string de formato, como no printf.
 * @param ... Argumentos variáveis para a string de formato.
 */
void tslog_log(ts_logger_t *logger, const char *level, const char *format, ...);

/**
 * @brief Fecha a biblioteca de logging.
 * Libera os recursos (fecha o arquivo e destrói o mutex).
 * * @param logger O ponteiro para o logger a ser destruído.
 */
void tslog_destroy(ts_logger_t *logger);

#endif // TSLOG_H