#!/bin/bash

make clean all
# Define a function to run the test
run_test() {
    local rows=$1
    local cols=$2
    local np=$3
    local matrix_file="matrix_${rows}x${cols}.dat"

    # Generate the matrix file
    ./make-2d $rows $cols $matrix_file

    # Run the MPI program with the specified number of processes
    echo "Running with matrix size ${rows}x${cols} and ${np} processes..."
    mpirun --oversubscribe -np $np ./test-mpi-read $matrix_file

    # Optionally print the matrix to verify correctness
    ./print-2d $matrix_file
}

# Test small matrices (10 rows, 6 columns)
for np in {1..10}; do
    run_test 10 6 $np
done

# Test larger matrices
for rows in 500 1000; do
    for cols in 100 200; do
        for np in {1..100}; do
            run_test $rows $cols $np
        done
        # Test with oversubscription for larger matrices
        for np in {1..200}; do
            run_test $rows $cols $np
        done
    done
done

echo "All tests completed!"