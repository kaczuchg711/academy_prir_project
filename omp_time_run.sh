#!/bin/bash

# Check if a file name is provided as an argument
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <text_file_name> <number_of_threads>"
    exit 1
fi

# Assign the first argument to a variable
TEXT_FILE="$1"
NUMBER_OF_THREADS="$2"

# Initialize sum of execution times
sum=0

# Run the program 10 times
for i in {1..10}
do
    # Compile and run the OpenMP version
    echo "Running XOR - OpenMP approach:"
    g++ -fopenmp XOR_omp.cpp -o XOR_omp
    start_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
    ./XOR_omp "$TEXT_FILE" "$NUMBER_OF_THREADS"
    end_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
    runtime=$((end_time - start_time))
    echo "Execution time: $runtime ms"
    sum=$((sum + runtime))
    echo

    # Remove the compiled files
    rm XOR_omp
done

# Calculate and print the mean execution time
mean=$((sum / 10))
echo "Mean execution time: $mean ms"