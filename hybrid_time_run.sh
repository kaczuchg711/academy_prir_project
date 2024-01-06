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

# Loop over the number of threads from 1 to 16
for NUMBER_OF_THREADS in $(seq 1 16)
do
    echo "Running with $NUMBER_OF_THREADS threads:"
    # Run the program 10 times
    for i in {1..10}
    do
        # Compile and run the hybrid version
        mpic++ -fopenmp XOR_hybrid.cpp -o XOR_hybrid
        start_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
        mpirun -np "$NUMBER_OF_PROCESS" ./XOR_hybrid "$TEXT_FILE" "$NUMBER_OF_THREADS"
        end_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
        runtime=$((end_time - start_time))
        sum=$((sum + runtime))

        # Remove the compiled files
        rm XOR_hybrid
    done

    # Calculate and print the mean execution time
    mean=$((sum / 10))
    echo "Mean execution time for $NUMBER_OF_THREADS threads: $mean ms"
    echo
    # Reset sum for the next iteration
    sum=0
done