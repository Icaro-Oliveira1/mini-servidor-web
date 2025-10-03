# Projeto Mini Servidor Web (HTTP em C)

Este documento contém as instruções completas para compilar, executar e testar os componentes do projeto Mini Servidor Web.

O projeto está dividido em duas partes principais:
1.  Uma biblioteca de logging thread-safe (`libtslog`).
2.  Um protótipo de servidor web concorrente que utiliza a biblioteca de log.

---

## Estrutura de Arquivos

O projeto está organizado na seguinte estrutura:

```
/
├── app/
│   └── servidor.c        # Código-fonte do servidor web HTTP
├── lib/
│   ├── tslog.c           # Implementação da biblioteca de logging
│   └── tslog.h           # Header com a interface da biblioteca
├── test/
│   └── test_log.c        # Programa para testar a libtslog
├── www/
│   └── index.html        # Página HTML simples para ser servida
├── Makefile                # Script de automação da compilação
└── test.sh                 # Script para testar o servidor com múltiplos clientes
```

---

## Etapa 1: Testando a Biblioteca de Logging (libtslog)

O objetivo desta etapa é validar que a biblioteca `libtslog` consegue receber mensagens de múltiplas threads simultaneamente sem corromper o arquivo de log.

1.  **Compile o programa de teste:**
    O `Makefile` está configurado para compilar tanto o servidor quanto o teste.
    ```sh
    make
    ```

2.  **Execute o programa de teste:**
    ```sh
    ./test_log
    ```

3.  **Resultado Esperado:**
    Um arquivo `test.log` será criado. Verifique se as mensagens de todas as threads estão completas e não há linhas corrompidas, provando que o mutex da biblioteca funciona.

---

## Etapa 2: Executando o Servidor Web

Esta etapa executa o protótipo do servidor web, que é capaz de atender requisições GET de forma concorrente.

1.  **Compile o servidor (se ainda não o fez):**
    ```sh
    make
    ```
    Este comando criará o executável `servidor`.

2.  **Inicie o servidor:**
    ```sh
    ./servidor
    ```
    O terminal ficará ocupado, com o servidor escutando por conexões na porta 8080. As atividades serão registradas no arquivo `server.log`.

3.  **Teste o servidor:**

    **A) Teste Manual com Navegador:**
    - Abra seu navegador de internet.
    - Acesse o endereço: `http://localhost:8080`
    - Você deverá ver a página `index.html`.

    **B) Teste de Carga com Múltiplos Clientes:**
    - Abra **um novo terminal** na mesma pasta do projeto.
    - Dê permissão de execução ao script: `chmod +x test.sh`
    - Execute o script: `./test.sh`

4.  **Resultado Esperado:**
    O script simulará múltiplos clientes. Verifique o arquivo `server.log` para ver o registro de todas as requisições, cada uma atendida por uma thread diferente.

---

## Limpeza do Projeto

Para remover todos os arquivos gerados pela compilação e os logs, utilize o comando `make clean`.

```sh
make clean
```