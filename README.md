============================================================
       Projeto Mini Servidor Web - Etapa 1: libtslog
============================================================

Este documento contém as instruções para compilar e executar a primeira etapa do projeto: a criação e o teste da biblioteca de logging thread-safe (`libtslog`).

O objetivo desta etapa é validar que a biblioteca consegue receber mensagens de múltiplas threads simultaneamente sem corromper o arquivo de log.


----------------------------------------
Estrutura de Arquivos
----------------------------------------

O projeto nesta etapa está organizado da seguinte forma:

/
├── lib/
│   ├── tslog.c           # Implementação da biblioteca de logging
│   └── tslog.h           # Header com a interface da biblioteca
├── test/
│   └── testlog.c        # Programa de teste com múltiplas threads
└── Makefile                # Script de automação da compilação


----------------------------------------
Como Compilar e Executar
----------------------------------------

Para compilar e rodar o teste, siga os passos abaixo no seu terminal.

1.  **Navegue até a pasta raiz do projeto.**

2.  **Compile o projeto usando o Makefile:**
    Execute o comando:
    ```sh
    make
    ```
    Este comando irá ler as regras do `Makefile`, compilar todos os arquivos `.c` necessários e criar um executável final chamado `test_log` na pasta raiz.

3.  **Execute o programa de teste:**
    Para rodar o teste que simula múltiplas threads escrevendo logs, execute:
    ```sh
    ./test_log
    ```

----------------------------------------
Resultado Esperado
----------------------------------------

Após executar o programa de teste, um novo arquivo chamado `test.log` será criado na pasta raiz do projeto.

O conteúdo deste arquivo é a prova do sucesso. Você deve observar que:
1.  As mensagens de log de todas as threads estarão presentes.
2.  **O mais importante:** Cada linha de log estará perfeitamente formatada e completa. Não haverá linhas "quebradas" ou com texto de diferentes threads misturado.

Isso demonstra que o `mutex` implementado em `libtslog` está funcionando corretamente, serializando o acesso ao arquivo e garantindo a segurança em um ambiente concorrente (thread-safe).

**Exemplo de saída no `test.log`:**
[2025-09-29 12:18:13] [SYSTEM] [ThreadID: 140176665368384] Logger inicializado. Criando 5 threads...
[2025-09-29 12:18:13] [INFO] [ThreadID: 140176665364160] Mensagem 1 do trabalhador 1.
[2025-09-29 12:18:13] [INFO] [ThreadID: 140176656971456] Mensagem 1 do trabalhador 2.
[2025-09-29 12:18:13] [INFO] [ThreadID: 140176648578752] Mensagem 1 do trabalhador 3.
[2025-09-29 12:18:13] [INFO] [ThreadID: 140176640186048] Mensagem 1 do trabalhador 4.
[2025-09-29 12:18:13] [INFO] [ThreadID: 140176631793344] Mensagem 1 do trabalhador 5.
...
[2025-09-29 12:18:13] [DEBUG] [ThreadID: 140176656971456] Trabalhador 2 terminou.
[2025-09-29 12:18:13] [DEBUG] [ThreadID: 140176640186048] Trabalhador 4 terminou.
[2025-09-29 12:18:13] [DEBUG] [ThreadID: 140176648578752] Trabalhador 3 terminou.
[2025-09-29 12:18:13] [DEBUG] [ThreadID: 140176665364160] Trabalhador 1 terminou.
[2025-09-29 12:18:13] [DEBUG] [ThreadID: 140176631793344] Trabalhador 5 terminou.
[2025-09-29 12:18:13] [SYSTEM] [ThreadID: 140176665368384] Teste concluído. Desligando o logger.


----------------------------------------
Limpeza do Projeto
----------------------------------------

Para remover todos os arquivos gerados durante a compilação (arquivos objeto `.o`, o executável `test_log`) e também o arquivo `test.log`, utilize o comando `make clean`.

Execute na pasta raiz:
```sh
make clean