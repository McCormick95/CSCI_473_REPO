#!/bin/bash

# Set up paths relative to project root
CODE_DIR="../code"
GRAPHS_DIR="../graphs"

OUT="out_temp"
MATRIX="temp_matrix"
DEBUG=3
ITER=12

CSV_FILE="stencil_benchmarks_local.csv"

# Create graphs directory if it doesn't exist
mkdir -p $GRAPHS_DIR

echo "RUN_TYPE,ROWS,COLS,P,ITER,TOTAL,WORK,OTHER" > $CSV_FILE

# Smaller array sizes for laptop testing
array_size=("5000" "10000" "20000")
# Using oversubscribe to allow more threads than physical cores
process_count=("1" "2" "4" "8")

# Change to code directory and build
cd $CODE_DIR || exit 1
make clean all

for array in "${array_size[@]}"; do
    echo "Processing matrix size: ${array}x${array}"
    
    # Generate the input matrix
    ./make-2d $array $array $MATRIX
    
    # Run serial version
    echo "  Running serial implementation..."
    output_serial=$(./stencil-2d $ITER $MATRIX $OUT $DEBUG)
    echo "SERIAL,$array,$array,1,$ITER,$output_serial" >> "../data/$CSV_FILE"

    for processes in "${process_count[@]}"; do
        echo "  Running parallel implementations with $processes threads..."
        
        # Run OpenMP version with oversubscribe
        export OMP_NUM_THREADS=$processes
        output_omp=$(./omp-stencil-2d $ITER $MATRIX $OUT $DEBUG $processes)
        echo "OMP,$array,$array,$processes,$ITER,$output_omp" >> "../data/$CSV_FILE"

        # Run Pthreads version
        output_pth=$(./pth-stencil-2d $ITER $MATRIX $OUT $DEBUG $processes)
        echo "PTH,$array,$array,$processes,$ITER,$output_pth" >> "../data/$CSV_FILE"
    done
    
    echo "Completed matrix size: ${array}x${array}"
    echo "----------------------------------------"
done

# Return to data directory
cd "../data" || exit 1

# Check if Python and required packages are available
if command -v python3 &> /dev/null; then
    if python3 -c "import pandas; import matplotlib" 2>/dev/null; then
        echo "Generating performance visualizations..."
        python3 make_graphs.py "$CSV_FILE" -o "$GRAPHS_DIR"
        
        if [ $? -eq 0 ]; then
            echo "Visualizations have been generated in the 'graphs' directory."
        else
            echo "Error generating visualizations."
        fi
    else
        echo "Warning: pandas and matplotlib are required for visualization."
        echo "Install them using: pip install pandas matplotlib"
    fi
else
    echo "Warning: Python 3 is not available. Skipping visualization generation."
fi

# Cleanup temporary files (in code directory)
rm -f "$CODE_DIR/$OUT" "$CODE_DIR/$MATRIX"

echo "Data collection complete. Results saved in $CSV_FILE"
echo "------------------------  GATHER DATA COMPLETE  ------------------------"