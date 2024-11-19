#!/bin/bash

# set -x

make clean all

./make-2d 7 7 A.dat
./print-2d A.dat

echo "---------------------------------------------------------------------"
# mpirun -np 2 valgrind ./test-mpi-read A.dat B.dat C.dat

mpirun -np 5 ./test-mpi-read A.dat B.dat C.dat

# ls -l
echo "---------------------------------------------------------------------"

diff B.dat C.dat

make clean

rm "A.dat"
rm "B.dat"
rm "C.dat"