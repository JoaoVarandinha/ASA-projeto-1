#!/bin/bash

# --- Configuração ---
PMAX=10000000000
SEED=42
OUTPUT_FILE="tempos_execucao.csv"

# Forçar a saída do comando 'time' a ser APENAS o tempo real (em segundos).
# O tempo será escrito para stderr por padrão (com time built-in).
TIMEFORMAT="%3R" # %3R é o formato para o tempo real arredondado para 3 casas decimais

# --- Compilação ---
echo "A compilar ASA-projeto1.cpp..."
g++ -std=c++17 ASA-projeto1.cpp -o ASA-projeto1 -O3 || { echo "Erro na compilação de ASA-projeto1.cpp."; exit 1; }

echo "A compilar gerador_p1.cpp..."
g++ -std=c++17 gerador_p1.cpp -o gerador_p1 -O3 || { echo "Erro na compilação de gerador_p1.cpp."; exit 1; }

# --- Execução e Medição de Tempo ---
echo "N;Tempo_Real_s" > "$OUTPUT_FILE" # Cabeçalho

echo "A correr os testes..."
# Defina o intervalo de N (cerca de 100 valores)
N_VALUES=$(seq 10 10 2000)

for N in $N_VALUES; do
    echo "Teste para N = $N"

    # 1. Medição de Tempo e Execução
    # O comando 'time' built-in é executado.
    # O output do 'time' (tempo, escrito para stderr) é capturado na variável TIME_OUTPUT.
    # O output do programa 'ASA-projeto1' (resultado, stdout) é redirecionado para /dev/null.
    # O output de 'gerador_p1' é piped para 'ASA-projeto1'.
    
    # Executa a subshell ( ... ) e captura o seu stderr (2>&1) para a variável.
    # O stdout do programa principal (1>/dev/null) é descartado.
    TIME_OUTPUT=$( { time ./gerador_p1 "$N" "$PMAX" "$SEED" | ./ASA-projeto1 1>/dev/null ; } 2>&1 )

    # 2. Extração do Tempo
    # Como definimos TIMEFORMAT="%3R", TIME_OUTPUT deve conter apenas o tempo.
    # Usamos tr -d para remover a nova linha e espaços.
    REAL_TIME=$(echo "$TIME_OUTPUT" | tr -d '[:space:]')
    
    # 3. Verificação e Escrita
    if [[ -z "$REAL_TIME" ]]; then
        REAL_TIME="ERRO_CAPTURA"
    fi
    
    echo "$N;$REAL_TIME" >> "$OUTPUT_FILE"
    
done

echo "Testes concluídos. Resultados guardados em $OUTPUT_FILE"

# --- Limpeza (Opcional) ---
# rm -f ASA-projeto1 gerador_p1