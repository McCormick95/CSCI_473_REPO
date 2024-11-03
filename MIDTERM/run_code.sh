#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

SERIAL_OUT="out_0"
PTH_OUT="out_1"
OMP_OUT="out_2"

SERIAL_ALL="all_0.RAW"
PTH_ALL="all_1.RAW"
OMP_ALL="all_2.RAW"

MATRIX="matrix_test"
ROWS=100
COLS=100
ITTR=10
DEBUG=0

make clean all

./make-2d $ROWS $COLS $MATRIX

# echo "INITIAL MATRIX -----------------------------------------------------------"
# ./print-2d $MATRIX

echo "--------------------------------------------------------------------------"
./stencil-2d $ITTR $MATRIX $SERIAL_OUT $DEBUG
./pth-stencil-2d $ITTR $MATRIX $PTH_OUT $DEBUG 3 
./omp-stencil-2d $ITTR $MATRIX $OMP_OUT $DEBUG 3 

# echo "--------------------------------------------------------------------------"
# ./print-2d $SERIAL_OUT
# echo "--------------------------------------------------------------------------"
# ./print-2d $PTH_OUT
# echo "--------------------------------------------------------------------------"
# ./print-2d $OMP_OUT
# echo "--------------------------------------------------------------------------"


echo "DIFFING OUTPUT FILES...."
if diff3 "$SERIAL_OUT" "$PTH_OUT" "$OMP_OUT" > /dev/null 2>&1; then
    echo -e "\t${GREEN}All three files are the same.${NC}"

    rm -f "$SERIAL_OUT" "$PTH_OUT" "$OMP_OUT" "$SERIAL_ALL" "$PTH_ALL" "$OMP_ALL" "$MATRIX"
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
echo "TESTING COMPLETE ---------------------------------------------------------"



