#!/bin/bash

make clean all

./make-2d 10 10 "example_1"

./print-2d "example_1" > "temp_1"

mpirun -np 5 ./test-mpi-read "example_1" > "temp_2"

cat "temp_1"
echo "--------------------------------------------------"
cat "temp_2"

echo "--------------------------------------------------"

if diff "temp_1" "temp_2" > /dev/null; then
        echo "TEST: The files are the same."
    else
        echo "TEST: The files are different."
fi



rm "example_1"
rm "temp_1"
rm "temp_2"
