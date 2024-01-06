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

# Run the program 10 times
for i in {1..10}
do
    #Compile and run the sequential version
    echo "Running XOR - sequential approach:"
    g++ XOR_sequential.cpp -o XOR_sequential
    start_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
    ./XOR_sequential "$TEXT_FILE"
    end_time=$(perl -MTime::HiRes -e 'printf("%.0f\n", Time::HiRes::time() * 1000)')
    runtime=$((end_time - start_time))
    echo "Execution time: $runtime ms"
    sum=$((sum + runtime))
    echo

    # Remove the compiled files
    rm XOR_sequential
done

# Calculate and print the mean execution time
mean=$((sum / 10))
echo "Mean execution time: $mean ms"