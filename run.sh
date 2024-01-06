#!/bin/bash

# Check if a file name is provided as an argument
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <text_file_name> <number_of_process> <number_of_threads>"
    exit 1
fi

# Assign the first argument to a variable
TEXT_FILE="$1"
NUMBER_OF_PROCESS="$2"
NUMBER_OF_THREADS="$3"

# Set the TIMEFORMAT to only display real time
TIMEFORMAT='Execution time (real): %R s'

#Compile and run the sequential version
echo "Running XOR - sequential approach:"
g++ XOR_sequential.cpp -o XOR_sequential
time ./XOR_sequential "$TEXT_FILE"
echo
# Compile and run the OpenMP version
echo "Running XOR - OpenMP approach:"
g++ -fopenmp XOR_omp.cpp -o XOR_omp
time ./XOR_omp "$TEXT_FILE" "$NUMBER_OF_THREADS"
echo
# Compile and run the MPI version
echo "Running XOR - MPI approach:"
mpic++ XOR_mpi.cpp -o XOR_mpi
time mpirun -np "$NUMBER_OF_PROCESS" ./XOR_mpi "$TEXT_FILE"
echo
# Compile and run the hybrid version
echo "Running XOR - hybrid approach:"
mpic++ -fopenmp XOR_hybrid.cpp -o XOR_hybrid
time mpirun -np "$NUMBER_OF_PROCESS" ./XOR_hybrid "$TEXT_FILE" "$NUMBER_OF_THREADS"

# Remove the compiled files
rm XOR_sequential XOR_omp XOR_mpi XOR_hybrid