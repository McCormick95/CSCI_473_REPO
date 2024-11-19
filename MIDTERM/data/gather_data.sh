#!/bin/bash

OUT="out_temp"

SERIAL_ALL="all_0.RAW"
PTH_ALL="all_1.RAW"
OMP_ALL="all_2.RAW"

MATRIX="temp_matrix"
DEBUG=3
ITER=1


CSV_FILE="stencil_benchmarks.csv"

echo "RUN_TYPE,ROWS,COLS,P,ITER,TOTAL,WORK,OTHER" > $CSV_FILE

array_size=("5000" "10000" "20000" "40000" )
process_count=("1" "2" "4" "8" "16")

make clean all

# echo "------------------------------------------------------------------------"

for array in "${array_size[@]}"; do
    # echo "---------------------------  RUNNING SERIAL  ---------------------------"
    ./make-2d $array $array $MATRIX
    output_serial=$(./stencil-2d $ITER $MATRIX $OUT $DEBUG)
    echo "SERIAL,$array,$array,1,$ITER,$output_serial" >> $CSV_FILE

    for processes in "${process_count[@]}"; do
        # echo "---------------------------  RUNNING OpenMP  ---------------------------"
        output_omp=$(./omp-stencil-2d $ITER $MATRIX $OUT $DEBUG $processes)
        echo "OMP,$array,$array,$processes,$ITER,$output_omp" >> $CSV_FILE

        # echo "--------------------------  RUNNING PTHREADS  --------------------------"
        output_pth=$(./pth-stencil-2d $ITER $MATRIX $OUT $DEBUG $processes)
        echo "PTH,$array,$array,$processes,$ITER,$output_pth" >> $CSV_FILE
    done
done

echo "------------------------  GATHER DATA COMPLETE  ------------------------"