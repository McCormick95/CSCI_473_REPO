#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

make clean all

echo "-----------------------------------------------------------------------"
./make-2d 10 10 "example_1"
echo -e "${BLUE}TESTING: 10 x 10 w/ np= 6${NC}"
echo "-----------------------------------------------------------------------"
mpirun --oversubscribe -np 6 ./test-mpi-read "example_1" "temp_2" > /dev/null

./print-2d "example_1" > "temp_1"
./print-2d "temp_2" > "temp_3"

cat "temp_1"
echo "-----------------------------------------------------------------------"
cat "temp_3"
echo "-----------------------------------------------------------------------"


echo -e "${BLUE}DIFFING PRINT FILES....${NC}"
if diff "temp_1" "temp_3" > /dev/null; then
    echo -e "\t${GREEN}Print files are the same.${NC}"
else
    echo -e "\t${RED}Print files are different.${NC}"
fi

echo -e "${BLUE}DIFFING BINARY FILES....${NC}"
if diff "example_1" "temp_2" > /dev/null; then
    echo -e "\t${GREEN}Binary files are the same.${NC}"
else
    echo -e "\t${RED}Binary files are different.${NC}"
fi
echo "-----------------------------------------------------------------------"

rm "example_1"
rm "temp_1"
rm "temp_2"
rm "temp_3"
echo -e "${BLUE}.....TEMP FILES REMOVED.....${NC}"
echo -e "${BLUE}.....EXAMPLE COMPLETE.......${NC}"