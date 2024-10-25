#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utilities.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <num threads> <matrix A file> <vector X file> <output Y file>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    const char *matrix_file = argv[2];
    const char *vector_file = argv[3];
    const char *output_file = argv[4];

    double **A;   // Matrix A
    double *X;    // Vector X
    double *Y;    // Output vector Y
    int rows, cols, vector_size;

    // Read matrix and vector from files
    read_matrix(matrix_file, &A, &rows, &cols);
    read_vector(vector_file, &X, &vector_size);

    // Ensure the matrix and vector dimensions are compatible
    if (cols != vector_size) {
        fprintf(stderr, "Error: Matrix columns (%d) and vector size (%d) do not match!\n", cols, vector_size);
        return 1;
    }

    // Allocate memory for the result vector
    Y = (double *)malloc(rows * sizeof(double));

    // Perform matrix-vector multiplication
    matrix_vector_multiply(A, X, Y, rows, cols, num_threads);

    // Write the result vector to the output file
    write_vector(output_file, Y, rows);

    // Free dynamically allocated memory
    for (int i = 0; i < rows; i++) {
        free(A[i]);
    }
    free(A);
    free(X);
    free(Y);

    return 0;
}


