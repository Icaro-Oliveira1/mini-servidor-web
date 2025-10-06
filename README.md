# Projeto Final: Mini Servidor Web Concorrente em C

Este projeto implementa um servidor web HTTP/1.0 simples, concorrente e gerenciável via linha de comando, desenvolvido em C utilizando pthreads e sockets POSIX.

## Funcionalidades
* **Servidor HTTP Concorrente:** Atende múltiplas requisições simultaneamente usando um modelo de "thread por conexão".
* **Serviço de Arquivos Estáticos:** Responde a requisições `GET`, servindo arquivos de um diretório `www/`.
* **Logging Thread-Safe:** Todas as atividades são registradas em `server.log` usando a biblioteca `libtslog`.
* **CLI de Gerenciamento:** O servidor pode ser iniciado, parado e verificado via linha de comando.
* **Desligamento Gracioso:** Encerra de forma limpa, registrando estatísticas finais.

---

## Estrutura de Arquivos
```
/
├── app/
│   └── servidor.c        # Código-fonte do servidor web e da CLI
├── lib/
│   ├── tslog.c           # Implementação da biblioteca de logging
│   └── tslog.h           # Header da biblioteca
├── www/
│   └── index.html        # Página HTML de exemplo
├── Makefile                # Script de compilação
└── test.sh                 # Script para teste de carga
```
---

## Como Compilar e Usar

**1. Compilar o Projeto**
Na pasta raiz, execute o comando `make`. Isso irá gerar o executável `servidor`.
```sh
make
```

**2. Gerenciando o Servidor (CLI)**

* **Iniciar o Servidor:**
    O servidor será iniciado em background. Um arquivo `server.pid` será criado para rastrear o processo.
    ```sh
    ./servidor start
    ```

* **Verificar o Status:**
    Verifica se o arquivo `server.pid` existe e mostra o ID do processo.
    ```sh
    ./servidor status
    ```

* **Parar o Servidor:**
    Envia um sinal de término para o processo do servidor, que irá desligar de forma limpa.
    ```sh
    ./servidor stop
    ```
    Ao parar, as estatísticas finais serão gravadas em `server.log`.

**3. Testando o Servidor**
Com o servidor rodando (`./servidor start`), você pode:
* Acessar `http://localhost:8080` no seu navegador.
* Executar o script de teste de carga: `chmod +x test.sh` e depois `./test.sh`.

---

## Limpeza
Para remover todos os arquivos gerados (executáveis, objetos, logs, pid), use:
```sh
make clean
```