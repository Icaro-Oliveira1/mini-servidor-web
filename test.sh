#!/bin/bash

echo "Iniciando teste de carga com 10 clientes simultâneos..."

# Limpa o log anterior para vermos apenas os resultados deste teste
> server.log

# Dispara 10 requisições em paralelo para o index.html
for i in $(seq 1 10)
do
   echo "Cliente $i fazendo requisição..."
   curl -s http://localhost:8080/ > /dev/null &
done

# Dispara uma requisição para um arquivo que não existe para testar o 404
echo "Cliente 11 fazendo requisição para arquivo inexistente..."
curl -s http://localhost:8080/naoexiste.html > /dev/null &

echo "Aguardando todas as requisições terminarem..."
wait
echo "Teste concluído. Verifique o arquivo 'server.log' para ver o registro das 11 requisições."