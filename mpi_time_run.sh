#!/bin/bash

# Check if a file name is provided as an argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <text_file_name>"
    exit 1
fi

# Assign the first argument to a variable
TEXT_FILE="$1"

# Set the TIMEFORMAT to only display real time
TIMEFORMAT='Execution time (real): %R s'

# Initialize sum of execution times
sum=0

# Loop over the number of processes from 2 to 8
for NUMBER_OF_PROCESS in $(seq 2 8)
do
    echo "Running with $NUMBER_OF_PROCESS processes:"
    # Run the program 10 times
    for i in {1..10}
    do
        # Compile and run the mpi version
        mpic++ XOR_mpi.cpp -o XOR_mpi
        start_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
        mpirun -np "$NUMBER_OF_PROCESS" ./XOR_mpi "$TEXT_FILE"
        end_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
        runtime=$((end_time - start_time))
        sum=$((sum + runtime))

        # Remove the compiled files
        rm XOR_mpi
    done

    # Calculate and print the mean execution time
    mean=$((sum / 10))
    echo "Mean execution time for $NUMBER_OF_PROCESS processes: $mean ms"
    echo
    # Reset sum for the next iteration
    sum=0
done