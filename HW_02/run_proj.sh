#!/bin/bash
set -x
make clean

make

OUTPUT_FILE="results.txt"

DIVISIONS="100000000 200000000 300000000 400000000 500000000"

for p in {1..16}; do
    for n in $DIVISIONS; do
        echo "P= $p, N=$n"
        mpirun -np $p --oversubscribe ./mpi_trap -n $n -a 0 -b 1000 -f 3 > temp.txt
        t=$(grep "T:" temp.txt | awk '{print $2}')
        echo "$p, $n, $t" >> $OUTPUT_FILE
    done
done

echo "All ouput written to results.txt"

