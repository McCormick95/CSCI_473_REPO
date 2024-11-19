#ifndef UTILITIES_H
#define UTILITIES_H

// Function prototypes
void read_matrix(const char *filename, double ***matrix, int *rows, int *cols);
void read_vector(const char *filename, double **vector, int *size);
void write_vector(const char *filename, double *vector, int size);
void matrix_vector_multiply(double **matrix, double *vector, double *result, int rows, int cols, int num_threads);

#endif // UTILITIES_H