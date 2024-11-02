#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

SERIAL_OUT="out_0"
PTH_OUT="out_1"
OMP_OUT="out_2"

MATRIX="matrix_test"
ROWS="10"
COLS="10"
ITTR="100"

make clean all

./make-2d $ROWS $COLS $MATRIX

./print-2d matrix_1


./stencil-2d $ITTR $MATRIX $SERIAL_OUT all_0.RAW
./pth-stencil-2d $ITTR $MATRIX $PTH_OUT 0 3 all_1.RAW
./omp-stencil-2d $ITTR $MATRIX $OMP_OUT 0 3 all_2.RAW

./print-2d $SERIAL_OUT
echo "--------------------------------------------------------------------------"
./print-2d $PTH_OUT
echo "--------------------------------------------------------------------------"
./print-2d $OMP_OUT
echo "--------------------------------------------------------------------------"

echo "DIFFING OUTPUT FILES...."
if diff3 "$SERIAL_OUT" "$PTH_OUT" "$OMP_OUT" > /dev/null 2>&1; then
    echo -e "\t${GREEN}All three files are the same.${NC}"
else
    # Additional checks to identify which files differ
    if diff "$SERIAL_OUT" "$PTH_OUT" > /dev/null 2>&1; then
        echo -e "\t${YELLOW}File 3 differs from files 1 and 2.${NC}"
    elif diff "$matrix_file" "$OMP_OUT" > /dev/null 2>&1; then
        echo -e "\t${YELLOW}File 2 differs from files 1 and 3.${NC}"
    elif diff "$OMP_OUT" "$PTH_OUT" > /dev/null 2>&1; then
        echo -e "\t${YELLOW}File 1 differs from files 2 and 3.${NC}"
    else
        echo -e "\t${RED}All three files are different.${NC}"
    fi
fi



