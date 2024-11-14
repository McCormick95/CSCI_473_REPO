#!/bin/bash

set -e

make clean all

rows=(16 32 64 128 256 512)
cols=(1 2 3 4 5)
processes=(1 2 3 4 5 8 16)

for r in "${rows[@]}"; do
    for c in "${cols[@]}"; do
        for p in "${processes[@]}"; do
            ./make-2d "$r" "$c" A.dat

            mpirun --oversubscribe -np "$p" ./test-mpi-read A.dat B.dat C.dat

            diff_results=$(diff B.dat C.dat)
            if [ -n "$diff_results" ]; then  # Check if diff_results is not empty
			    echo "Diff results:"
			    echo "$diff_results"  # Print the diff results
			else
			    echo "No differences found."
			fi
        done
    done
done

echo "Testing Completed"

make clean 

rm "A.dat"
rm "B.dat"
rm "C.dat"