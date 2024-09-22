#!/bin/bash
set -x
make clean

make

DIVISIONS = "100000000 200000000 300000000 400000000 500000000"

for p in {1..16}; do
    for n in $DIVISIONS; do
        echo "P= $p, N=$n"
        mpirun -np $p ./mpi_trap -n $n -a 0 -b 1000 -f 3 > temp.txt
        TIME = 
    done
done

echo "All ouput written to output.txt"

