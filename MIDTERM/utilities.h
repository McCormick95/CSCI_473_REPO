#ifndef UTILITIES_H
#define UTILITIES_H

#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)

typedef struct{
    double **a;
    double **b;
    int start_row;
    int end_row;
    int cols;
    pthread_barrier_t *barrier;
    int iterations;
} ThreadData;

void malloc2D(double ***a, int jmax, int imax);
void apply_stencil_serial(double ***A, double ***B, int rows, int cols);
void write_file(double ***A, int rows, int cols, int ittr, FILE *f_name, int flag);
// void pth_apply_stencil(double ***A, double ***B, int rows, int cols);
void *pth_apply_stencil(void *arg);
void apply_stencil(double ***A, double ***B, int rows, int cols, int iterations, int thread_count);

#endif // UTILITIES_H