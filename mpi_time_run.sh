#!/bin/bash

# Check if a file name is provided as an argument
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <text_file_name> <number_of_process>"
    exit 1
fi

# Assign the first argument to a variable
TEXT_FILE="$1"
NUMBER_OF_PROCESS="$2"

# Set the TIMEFORMAT to only display real time
TIMEFORMAT='Execution time (real): %R s'

# Initialize sum of execution times
sum=0

# Run the program 10 times
for i in {1..10}
do
    # Compile and run the MPI version
    echo "Running XOR - MPI approach:"
    mpic++ XOR_mpi.cpp -o XOR_mpi
    start_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
    time mpirun -np "$NUMBER_OF_PROCESS" ./XOR_mpi "$TEXT_FILE"
    end_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
    runtime=$((end_time - start_time))
    echo "Execution time: $runtime ms"
    sum=$((sum + runtime))
    echo

    # Remove the compiled files
    rm XOR_mpi
done

# Calculate and print the mean execution time
mean=$((sum / 10))
echo "Mean execution time: $mean ms"