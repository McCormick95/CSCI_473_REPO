#!/bin/bash

# set -x

make clean all

./make-2d 8 8 A.dat
./print-2d A.dat

echo "---------------------------------------------------------------------"
# mpirun -np 2 valgrind ./test-mpi-read A.dat B.dat C.dat

mpirun -np 4 ./test-mpi-read A.dat B.dat C.dat

# ls -l
echo "---------------------------------------------------------------------"

diff B.dat C.dat

make clean