#!/bin/bash

# Check if a file name is provided as an argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <text_file_name>"
    exit 1
fi

# Assign the first argument to a variable
TEXT_FILE="$1"

# Initialize sum of execution times
sum=0

# Loop over the number of threads from 1 to 16
for NUMBER_OF_THREADS in $(seq 1 16)
do
    echo "Running with $NUMBER_OF_THREADS threads:"
    # Run the program 10 times
    for i in {1..10}
    do
        # Compile and run the omp version
        g++ -fopenmp XOR_omp.cpp -o XOR_omp
        start_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
        ./XOR_omp "$TEXT_FILE" "$NUMBER_OF_THREADS"
        end_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
        runtime=$((end_time - start_time))
        sum=$((sum + runtime))

        # Remove the compiled files
        rm XOR_omp
    done

    # Calculate and print the mean execution time
    mean=$((sum / 10))
    echo "Mean execution time for $NUMBER_OF_THREADS threads: $mean ms"
    echo
    # Reset sum for the next iteration
    sum=0
done