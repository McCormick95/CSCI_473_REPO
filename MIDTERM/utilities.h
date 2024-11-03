#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <omp.h>
#include "my_barrier.h"

#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)

typedef struct {
    double **curr;  // Current array being read from
    double **next;  // Next array being written to
} SharedArrays;

typedef struct {
    SharedArrays *shared;  // Shared array pointers
    int start_row;
    int end_row;
    int rows;
    int cols;
    int iterations;
    FILE *output_file;
    my_barrier_t *barrier;
} ThreadData;

// Memory allocation
void malloc2D(double ***a, int jmax, int imax);

void apply_stencil_serial(double ***A, double ***B, int rows, int cols);

void write_file(double ***A, int rows, int cols, int ittr, FILE *f_name, int flag);

void omp_apply_stencil(double ***A, double ***B, int rows, int cols);

void *pth_apply_stencil(void *arg);
void run_pth_stencil(double ***A, double ***B, int rows, int cols, int ittr, int thread_count, FILE *output_file);

#endif // UTILITIES_H