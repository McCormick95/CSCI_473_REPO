#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "utilities.h"
#include "timer.h"

void malloc2D(double ***a, int jmax, int imax){
    double **x = (double **)malloc(jmax*sizeof(double *) + jmax*imax*sizeof(double));

    x[0] = (double *)x + jmax;

    for(int j=1; j < jmax; j++){
        x[j] = x[j-1] + imax;
    }

    *a = x;
}

// SERIAL
void apply_stencil(double ***A, double ***B, int rows, int cols){
    double **a = *A;
    double **b = *B;

    // Only process interior points to avoid boundary issues
    for(int i = 1; i < rows-1; i++){
        for(int j = 1; j < cols-1; j++){
             double sum =   a[i-1][j-1] + a[i-1][j] + a[i-1][j+1] + 
                            a[i][j-1]   + a[i][j]   + a[i][j+1] + 
                            a[i+1][j-1] + a[i+1][j] + a[i+1][j+1];
            b[i][j] = sum / 9.0;
        }
    }
}

// PTHREADS
void *pth_apply_stencil(void *arg){
    ThreadData *data = (ThreadData)arg;
    
    for(int iter = 0; iter < data->iterations; iter++){
        // Only process interior points to avoid boundary issues
        for(int i = data->start_row; i < data->end_row; i++){
            for(int j = 1; j < data->cols-1; j++){
                double sum =data->a[i-1][j-1] + data->a[i-1][j] + data->a[i-1][j+1] + 
                            data->a[i][j-1]   + data->a[i][j]   + data->a[i][j+1]   + 
                            data->a[i+1][j-1] + data->a[i+1][j] + data->a[i+1][j+1];
                data->b[i][j] = sum / 9.0;
            }
        }
    }

    pthread_barrier_wait(data->barrier);

    if (data->start_row == 1) {  // Thread 0
        double **temp = data->a;
        data->a = data->b;
        data->b = temp;
    }
    

}

// flag = 0 --> printing one snapshot for all-iterations
//        1 --> printing intital state and metadata for all-iterations
//        2 --> printing final state and metadata
void write_file(double ***A, int rows, int cols, int ittr, FILE *f_name, int flag){
    double **a = *A;

    if(flag == 0){
        if(fwrite(a[0], sizeof(double), rows*cols, f_name) != (size_t)(rows*cols)){
            perror("ERROR: WHILE READING DOUBLES");
            fclose(f_name);
            exit(1);
        }
    }
    else if(flag == 1){
        if(fwrite(&rows, sizeof(int), 1, f_name) != 1 || fwrite(&cols, sizeof(int), 1, f_name) != 1 \
                    || fwrite(&ittr, sizeof(int), 1, f_name) != 1){
            perror("ERROR: WHILE WRITING METADATA");
            fclose(f_name);
            exit(1);
        }
    }
    else if(flag == 2){
        if(fwrite(&rows, sizeof(int), 1, f_name) != 1 || fwrite(&cols, sizeof(int), 1, f_name) != 1 ){
            perror("ERROR: WHILE WRITING METADATA");
            fclose(f_name);
            exit(1);
        }

        if(fwrite(a[0], sizeof(double), rows*cols, f_name) != (size_t)(rows*cols)){
            perror("ERROR: WHILE READING DOUBLES");
            fclose(f_name);
            exit(1);
        }
    }
}

