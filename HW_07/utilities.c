#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utilities.h"

// Function to read matrix from a binary file
void read_matrix(const char *filename, double ***matrix, int *rows, int *cols) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening matrix file");
        exit(EXIT_FAILURE);
    }
    // Read the metadata (number of rows and columns)
    fread(rows, sizeof(int), 1, file);
    fread(cols, sizeof(int), 1, file);
    // Allocate memory for the matrix
    *matrix = (double **)malloc((*rows) * sizeof(double *));
    for (int i = 0; i < *rows; i++) {
        (*matrix)[i] = (double *)malloc((*cols) * sizeof(double));
    }
    // Read matrix data (row-major format)
    for (int i = 0; i < *rows; i++) {
        fread((*matrix)[i], sizeof(double), *cols, file);
    }
    fclose(file);
}
// Function to read vector from a binary file
void read_vector(const char *filename, double **vector, int *size) {
    int dummyCols;
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening vector file");
        exit(EXIT_FAILURE);
    }
    // Read the metadata (vector size)
    fread(size, sizeof(int), 1, file);
    fread(&dummyCols, sizeof(int), 1, file);
    // Allocate memory for the vector
    *vector = (double *)malloc((*size) * sizeof(double));
    // Read vector data
    fread(*vector, sizeof(double), *size, file);
    fclose(file);
}
// Function to write vector to a binary file
void write_vector(const char *filename, double *vector, int size) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }
    // Write the result vector size followed by the vector data
    fwrite(&size, sizeof(int), 1, file);
    fwrite(vector, sizeof(double), size, file);
    fclose(file);
}
// Function to perform matrix-vector multiplication
void matrix_vector_multiply(double **matrix, double *vector, double *result, int rows, int cols, int num_threads) {
    #pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < rows; i++) {
        result[i] = 0.0;
        for (int j = 0; j < cols; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}



