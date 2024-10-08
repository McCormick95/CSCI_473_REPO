#!/bin/bash
BLUE='\033[0;34m'
make clean all
# Define a function to run the test
run_test() {
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    NC='\033[0m' # No Color

    local rows=$1
    local cols=$2
    local np=$3
    local matrix_file="matrix_${rows}x${cols}.dat"
    local temp_file_1="temp_file_make_2d"
    # local temp_file_2="temp_file_write"
    local temp_file_3="temp_file_test_mpi_out"
    local temp_file_4="temp_file_write_printed"

    # Generate the matrix file
    ./make-2d $rows $cols $matrix_file

    # Run the MPI program with the specified number of processes
    echo "TESTING: ${rows} x ${cols} w/ np= ${np}"
    mpirun --oversubscribe -np $np ./test-mpi-read $matrix_file $temp_file_3 > /dev/null

    # Optionally print the matrix to verify correctness
    ./print-2d $matrix_file > $temp_file_1
    ./print-2d $temp_file_3 > $temp_file_4

    # Use diff to compare the files
    # echo "------------------------------------------------------------------"
    # echo "TESTING: ${rows}x${cols} w/ ${np}:"
    echo "DIFFING PRINT FILES...."
    if diff "$temp_file_1" "$temp_file_4" > /dev/null; then
        echo -e "\t${GREEN}Print files are the same.${NC}"
    else
        echo -e "\t${RED}Print files are different.${NC}"
    fi

    echo "DIFFING BINARY FILES...."
    if diff "$matrix_file" "$temp_file_3" > /dev/null; then
        echo -e "\t${GREEN}Binary files are the same.${NC}"
    else
        echo -e "\t${RED}Binary files are different.${NC}"
    fi
    echo "------------------------------------------------------------------"

    rm "matrix_${rows}x${cols}.dat"
}


row_array=("50" "100" "500" "1000")
col_array=("50" "100" "500" "1000")

# Test larger matrices
for rows in "${row_array[@]}"; do
    for cols in "${col_array[@]}"; do
        for np in {1..16}; do
            run_test $rows $cols $np
        done
    done
done

rm "temp_file_make_2d"
# rm "temp_file_write"
rm "temp_file_test_mpi_out"
rm "temp_file_write_printed"


echo -e "${BLUE}.....ALL TESTS COMPLETE.....${NC}"