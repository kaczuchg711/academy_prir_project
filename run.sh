#!/bin/bash

# Funkcja do uruchamiania programu XOR_hybrid
run_xor_hybrid() {
    local file=$1
    local processes=$2
    local threads=$3

    # Kompilacja programu XOR_hybrid (dostosuj ścieżkę lub polecenie kompilacji według potrzeb)
    mpic++ -fopenmp XOR_hybrid.cpp -o XOR_hybrid

    # Uruchomienie programu i pomiar czasu
    echo -n "File: $file, Processes: $processes, Threads: $threads, Time (ms): "
    TIMEFORMAT=%R
    time (mpirun -np "$processes" ./XOR_hybrid "$file" "$threads") 2>&1
}

# Tablica zawierająca różne rozmiary plików
FILES=("1_4_bible.txt" "1_2_bible.txt" "3_4_bible.txt" "bible.txt")

# Zakres liczby procesów i wątków
PROCESSES=(2 3 4 5 6 7 8)
THREADS=(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16)

# Pętla dla każdego pliku
for file in "${FILES[@]}"; do
    # Pętla dla każdej liczby procesów
    for proc in "${PROCESSES[@]}"; do
        # Pętla dla każdej liczby wątków
        for thread in "${THREADS[@]}"; do
            run_xor_hybrid "$file" "$proc" "$thread"
        done
    done
done