#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Set up paths relative to project root
CODE_DIR="../code"
GRAPHS_DIR="../graphs"

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
DEBUG=3

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}Python 3 is not installed. Cannot generate visualizations.${NC}"
    exit 1
fi

# Check if required Python packages are installed
python3 -c "import pandas; import matplotlib" 2>/dev/null
if [ $? -ne 0 ]; then
    echo -e "${RED}Required Python packages (pandas, matplotlib) are not installed.${NC}"
    echo "Please install them using: pip install pandas matplotlib"
    exit 1
fi

# Create graphs directory if it doesn't exist
mkdir -p $GRAPHS_DIR

# Change to code directory and build
cd $CODE_DIR || exit 1
make clean all

./make-2d $ROWS $COLS $MATRIX

echo "--------------------------------------------------------------------------"
echo "Running correctness tests..."
./stencil-2d $ITTR $MATRIX $SERIAL_OUT $DEBUG
./pth-stencil-2d $ITTR $MATRIX $PTH_OUT $DEBUG 3 
./omp-stencil-2d $ITTR $MATRIX $OMP_OUT $DEBUG 3 

echo "DIFFING OUTPUT FILES...."
if diff3 "$SERIAL_OUT" "$PTH_OUT" "$OMP_OUT" > /dev/null 2>&1; then
    echo -e "\t${GREEN}All three files are the same.${NC}"

    rm -f "$SERIAL_OUT" "$PTH_OUT" "$OMP_OUT" "$SERIAL_ALL" "$PTH_ALL" "$OMP_ALL" "$MATRIX"
else
    # Additional checks to identify which files differ
    if diff "$SERIAL_OUT" "$PTH_OUT" > /dev/null 2>&1; then
        echo -e "\t${YELLOW}File 3 differs from files 1 and 2.${NC}"
    elif diff "$SERIAL_OUT" "$OMP_OUT" > /dev/null 2>&1; then
        echo -e "\t${YELLOW}File 2 differs from files 1 and 3.${NC}"
    elif diff "$OMP_OUT" "$PTH_OUT" > /dev/null 2>&1; then
        echo -e "\t${YELLOW}File 1 differs from files 2 and 3.${NC}"
    else
        echo -e "\t${RED}All three files are different.${NC}"
    fi
fi

# Return to data directory
cd "../data" || exit 1

echo -e "\n${GREEN}Running performance benchmarks...${NC}"
if [ -f "gather_data_local.sh" ]; then
    chmod +x gather_data_local.sh
    ./gather_data_local.sh
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Performance data collection complete.${NC}"
        
        if [ -f "stencil_benchmarks_local.csv" ]; then
            echo -e "\n${GREEN}Generating performance visualizations...${NC}"
            python3 make_graphs.py stencil_benchmarks_local.csv -o "$GRAPHS_DIR"
            
            if [ $? -eq 0 ]; then
                echo -e "${GREEN}Visualizations have been generated in the 'graphs' directory.${NC}"
            else
                echo -e "${RED}Error generating visualizations.${NC}"
            fi
        else
            echo -e "\n${RED}Error: stencil_benchmarks_local.csv not found after data collection.${NC}"
        fi
    else
        echo -e "${RED}Error during performance data collection.${NC}"
    fi
else
    echo -e "${RED}Error: gather_data_local.sh script not found.${NC}"
fi

echo "TESTING COMPLETE ---------------------------------------------------------"