#!/bin/bash

make clean all
# Define a function to run the test
run_test() {
    local rows=$1
    local cols=$2
    local np=$3
    local matrix_file="matrix_${rows}x${cols}.dat"
    local temp_file_1="temp_file_1"
    local temp_file_2="temp_file_2"

    # Generate the matrix file
    ./make-2d $rows $cols $matrix_file

    # Run the MPI program with the specified number of processes
    echo "Running with matrix size ${rows}x${cols} and ${np} processes..."
    mpirun --oversubscribe -np $np ./test-mpi-read $matrix_file > $temp_file_1

    # Optionally print the matrix to verify correctness
    ./print-2d $matrix_file > $temp_file_2

    # Use diff to compare the files
    if diff "$temp_file_1" "$temp_file_2" > /dev/null; then
        echo "TEST ${rows}x${cols} w/ ${np}: The files are the same."
    else
        echo "TEST ${rows}x${cols} w/ ${np}: The files are different."
    fi

    rm "matrix_${rows}x${cols}.dat"
}


row_array=("100" "500" "1000")

# Test larger matrices
for rows in "${row_array[@]}"; do
    for np in {1..100}; do
        cols=100
        run_test $rows $cols $np
    done
done

rm "temp_file_1"
rm "temp_file_2"


echo "All tests completed!"