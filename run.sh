#!/bin/bash

# Check if a file name is provided as an argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <text_file_name>"
    exit 1
fi

# Assign the first argument to a variable
TEXT_FILE="$1"

# Compile and run the sequential version
echo "Running XOR - sequential approach:"
g++ XOR_sequential.cpp -o XOR_sequential
./XOR_sequential "$TEXT_FILE"

# Compile and run the OpenMP version
echo "\nRunning XOR - OpenMP approach:"
g++ XOR_omp.cpp -fopenmp -o XOR_omp
./XOR_omp "$TEXT_FILE"

# Compile and run the MPI version
echo "\nRunning XOR - MPI approach:"
mpic++ XOR_mpi.cpp -o XOR_mpi
mpirun -np 8 ./XOR_mpi "$TEXT_FILE"
