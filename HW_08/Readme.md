# Parallel Matrix Operations with MPI

This project implements parallel matrix operations using MPI (Message Passing Interface). It focuses on row-striped matrix distribution with halo rows for distributed computing environments.

## Overview

The project provides functionality for:
- Creating 2D matrices and storing them in binary files
- Reading/writing matrices in a distributed manner using MPI
- Supporting halo rows for parallel processing
- Verifying correct matrix distribution and reconstruction

## Files

### Core Components
- `functions.c/h`: Core implementation of matrix operations and MPI utilities
- `make-2d.c`: Program to create and initialize 2D matrices
- `print-2d.c`: Program to display matrices from binary files
- `test-mpi-read.c`: Main test program for parallel matrix operations

### Build and Test
- `Makefile`: Compilation rules and dependencies
- `test_script.bash`: Basic testing script for 8x8 matrices
- `test_run_all.sh`: Comprehensive testing script with various matrix sizes and process counts

## Requirements

- MPI implementation (e.g., OpenMPI)
- GCC compiler
- Make build system
- Valgrind (optional, for memory testing)

## Building

To compile all components:
```bash
make clean all
```

## Usage

### Creating a Matrix
```bash
./make-2d <rows> <cols> <output_file>
```
Example:
```bash
./make-2d 8 8 A.dat
```

### Displaying a Matrix
```bash
./print-2d <input_file>
```
Example:
```bash
./print-2d A.dat
```

### Running MPI Tests
```bash
mpirun -np <processes> ./test-mpi-read <input_file> <output_file1> <output_file2>
```
Example:
```bash
mpirun -np 4 ./test-mpi-read A.dat B.dat C.dat
```

### Running Test Scripts

Basic test (8x8 matrix):
```bash
./test_script.bash
```

Comprehensive testing:
```bash
./test_run_all.sh
```

## Testing Matrix Sizes

The test_run_all.sh script tests combinations of:
- Rows: 16, 32, 64, 128, 256, 512
- Columns: 1, 2, 3, 4, 5
- Processes: 1, 2, 3, 4, 5, 8, 16

## Implementation Details

The project implements row-striped matrix distribution with halo rows:
- Each process receives a portion of the matrix rows
- Processes (except first and last) receive additional halo rows
- Halo rows contain -1.0 values for boundary conditions
- Process 0 has bottom halo only
- Middle processes have both top and bottom halos
- Last process has top halo only

## Memory Management

The project includes proper memory management:
- Dynamic allocation for matrix storage
- Cleanup of allocated resources
- Valgrind support for memory testing

## Notes

- Matrix files are stored in binary format
- The project supports double-precision floating-point values
- Error checking is implemented for file operations and MPI communications

