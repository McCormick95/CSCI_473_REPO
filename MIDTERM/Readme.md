# 2D Stencil Method Performance Analysis

This repository contains an implementation and performance analysis of a 2D stencil computation using three different approaches: serial, OpenMP, and Pthreads. The project compares the performance characteristics of each implementation across different problem sizes and thread counts.

## Project Structure

```
.
├── code/
│   ├── Makefile               # Build configuration
│   ├── make-2d.c              # Creates input matrices
│   ├── my_barrier.c           # Custom barrier implementation
│   ├── my_barrier.h           # Barrier header file
│   ├── omp-stencil-2d.c       # OpenMP implementation
│   ├── pth-stencil-2d.c       # Pthreads implementation
│   ├── stencil-2d.c           # Serial implementation
│   ├── print-2d.c             # Utility to print matrices
│   ├── timer.h                # Timing functions
│   ├── utilities.c            # Common utility functions
│   └── utilities.h            # Utility header file
├── data/
│   ├── gather_data.sh         # HPC benchmark data collection script
│   ├── gather_data_local.sh   # Local benchmark data collection script
│   ├── make_graphs.py         # Visualization script
│   ├── rm_batch_script.sh     # Batch job submission script
│   ├── run_code.sh            # Testing and benchmarking script
│   └── stencil_benchmarks_EXPANSE.csv  # Sample HPC benchmark results
├── graphs/                    # Generated performance graphs
├── presentation/              # Presentation materials
│   └── CSCI_473_MIDTERM.pptx  # Project presentation slides
└── report/                    # Analysis report and source files
```

## Building the Project

Navigate to the `code` directory and use the provided Makefile:

```bash
cd code
make clean all
```

This will build all executables:
- `make-2d`: Matrix generator
- `print-2d`: Matrix printer
- `stencil-2d`: Serial implementation
- `pth-stencil-2d`: Pthreads implementation
- `omp-stencil-2d`: OpenMP implementation

## Running the Code

### Basic Usage

Each implementation follows this general usage pattern:

```bash
# Serial version
./stencil-2d <iterations> <input_file> <output_file> <debug_level> [all_iterations_file]

# Pthreads version
./pth-stencil-2d <iterations> <input_file> <output_file> <debug_level> <num_threads> [all_iterations_file]

# OpenMP version
./omp-stencil-2d <iterations> <input_file> <output_file> <debug_level> <num_threads> [all_iterations_file]
```

Parameters:
- `iterations`: Number of stencil iterations to perform
- `input_file`: Input matrix file (RAW format)
- `output_file`: Output matrix file (RAW format)
- `debug_level`: Debug output level (0-3)
- `num_threads`: Number of threads (for parallel versions)
- `all_iterations_file`: Optional file to store intermediate results

### Running Benchmarks

The project provides two benchmark scripts:

#### Local Testing (Laptops/Desktops)
```bash
cd data
./run_code.sh
```

This script will:
1. Run correctness tests
2. Execute benchmarks using `gather_data_local.sh` with laptop-friendly settings:
   - Matrix sizes: 5000x5000, 10000x10000, 20000x20000
   - Thread counts: 1, 2, 4, 8
3. Generate visualizations automatically in the `graphs` directory

#### HPC Environment Testing
```bash
cd data
sbatch rm_batch_script.sh
```

This submits a job that runs `gather_data.sh` with full-scale tests:
- Matrix sizes: 5000x5000, 10000x10000, 20000x20000, 40000x40000
- Thread counts: 1, 2, 4, 8, 16

### Requirements for Local Testing

- Python 3
- Python packages: pandas, matplotlib
- GCC compiler with OpenMP support
- Sufficient RAM for matrix operations (at least 8GB recommended)

Install Python dependencies:
```bash
pip install pandas matplotlib
```

### Generating Visualizations

Visualizations are automatically generated after running benchmarks. To manually generate them:

```bash
cd data
python make_graphs.py stencil_benchmarks_local.csv -o ../graphs
```

This will create various performance graphs in the `graphs` directory, including:
- Execution time comparisons
- Speedup analysis
- Efficiency metrics
- Time component breakdowns

## Debug Levels

The implementations support different debug levels:
- 0: No debug output
- 1: Basic information (matrix size, iterations, etc.)
- 2: Detailed output including intermediate matrices
- 3: Performance timing data only

## File Formats

### Matrix File Format
- First 8 bytes: Two 32-bit integers for rows and columns
- Remaining bytes: Double-precision floating-point values in row-major order

### Benchmark CSV Format
Columns:
- RUN_TYPE: SERIAL, PTH (Pthreads), or OMP (OpenMP)
- ROWS: Matrix height
- COLS: Matrix width
- P: Number of threads
- ITER: Number of iterations
- TOTAL: Total execution time
- WORK: Computation time
- OTHER: Overhead time

## License

This project is available under the MIT License. See the LICENSE file for more details.