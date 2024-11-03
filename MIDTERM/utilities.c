#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <pthread.h>
#include "my_barrier.h"
#include "utilities.h"

void print_array(double ***A, int rows, int cols, int iter){
    double **a = *A;
    printf("ITERATION: %d ------------------------------------------------------- \n", iter);
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            printf("%.2f ", a[i][j]); 
        }
        printf("\n");
    }
}

void malloc2D(double ***a, int jmax, int imax) {
    double **x = (double **)malloc(jmax*sizeof(double *) + jmax*imax*sizeof(double));
    x[0] = (double *)x + jmax;
    
    for(int j=1; j < jmax; j++) {
        x[j] = x[j-1] + imax;
    }
    
    *a = x;
}

void apply_stencil_serial(double ***A, double ***B, int rows, int cols) {
    double **a = *A;
    double **b = *B;

    // Only process interior points to avoid boundary issues
    for(int i = 1; i < rows-1; i++) {
        for(int j = 1; j < cols-1; j++) {
            double sum = a[i-1][j-1] + a[i-1][j] + a[i-1][j+1] + 
                        a[i][j-1]    + a[i][j]   + a[i][j+1]   + 
                        a[i+1][j-1]  + a[i+1][j] + a[i+1][j+1];
            b[i][j] = sum / 9.0;
        }
    }
}

void write_file(double ***A, int rows, int cols, int ittr, FILE *f_name, int flag) {
    double **a = *A;

    if(flag == 0) {
        if(fwrite(a[0], sizeof(double), rows*cols, f_name) != (size_t)(rows*cols)) {
            perror("ERROR: WHILE READING DOUBLES");
            fclose(f_name);
            exit(1);
        }
    }
    else if(flag == 1) {
        if(fwrite(&rows, sizeof(int), 1, f_name) != 1 || 
           fwrite(&cols, sizeof(int), 1, f_name) != 1 ||
           fwrite(&ittr, sizeof(int), 1, f_name) != 1) {
            perror("ERROR: WHILE WRITING METADATA");
            fclose(f_name);
            exit(1);
        }
    }
    else if(flag == 2) {
        if(fwrite(&rows, sizeof(int), 1, f_name) != 1 || 
           fwrite(&cols, sizeof(int), 1, f_name) != 1) {
            perror("ERROR: WHILE WRITING METADATA");
            fclose(f_name);
            exit(1);
        }

        if(fwrite(a[0], sizeof(double), rows*cols, f_name) != (size_t)(rows*cols)) {
            perror("ERROR: WHILE READING DOUBLES");
            fclose(f_name);
            exit(1);
        }
    }
}

void omp_apply_stencil(double ***A, double ***B, int rows, int cols) {
    double **a = *A;
    double **b = *B;

    #pragma omp for collapse(2) schedule(static)
    // Only process interior points to avoid boundary issues
    for(int i = 1; i < rows-1; i++) {
        for(int j = 1; j < cols-1; j++) {
            double sum = a[i-1][j-1] + a[i-1][j] + a[i-1][j+1] + 
                        a[i][j-1]    + a[i][j]   + a[i][j+1]   + 
                        a[i+1][j-1]  + a[i+1][j] + a[i+1][j+1];
            b[i][j] = sum / 9.0;
        }
    }
}

void *pth_apply_stencil(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    
    for(int iter = 0; iter < data->iterations; iter++) {
        // Process assigned rows
        for(int i = data->start_row; i <= data->end_row; i++) {
            for(int j = 1; j < data->cols-1; j++) {
                double sum = (*(data->A))[i-1][j-1] + (*(data->A))[i-1][j] + (*(data->A))[i-1][j+1] + 
                             (*(data->A))[i][j-1]   + (*(data->A))[i][j]   + (*(data->A))[i][j+1]   + 
                             (*(data->A))[i+1][j-1] + (*(data->A))[i+1][j] + (*(data->A))[i+1][j+1];
                (*(data->B))[i][j] = sum / 9.0;
            }
        }
        
        // First barrier: wait for all calculations to complete
        my_barrier_wait(data->barrier);
        
        // Thread 0 handles pointer swap and file output
        if (data->start_row == 1) {  // Thread 0
            // Write to file if needed
            if (data->print_all_status == 1) {
                write_file(data->B, data->rows, data->cols, data->iterations, data->output_file, 0);
            }
            
            if(data->debug_flag == 2){
                print_array(data->B, data->rows, data->cols, iter);
            }
            
            // Swap pointers
            double **temp = *(data->A);
            *(data->A) = *(data->B);
            *(data->B) = temp;
        }
        
        // Second barrier: wait for swap to complete
        my_barrier_wait(data->barrier);
    }
    
    return NULL;
}

void run_pth_stencil(double ***M_A, double ***M_B, int rows, int cols, int ittr, int thread_count, FILE *output_file, int print_all_status, int debug_flag) {
    double **a = *M_A;
    double **b = *M_B;

    my_barrier_t barrier;
    my_barrier_init(&barrier, 0, thread_count);

    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    ThreadData *thread_data = malloc(thread_count * sizeof(ThreadData));

    // Initialize boundaries for array B
    for(int j = 0; j < cols; j++) {
        b[0][j] = a[0][j];
        b[rows-1][j] = a[rows-1][j];
    }
    for(int i = 0; i < rows; i++) {
        b[i][0] = a[i][0];
        b[i][cols-1] = a[i][cols-1];
    }

    *M_A = a;
    *M_B = b;

    for(int i = 0; i < thread_count; i++) {
        thread_data[i].A = M_A;
        thread_data[i].B = M_B;
        thread_data[i].cols = cols;
        thread_data[i].rows = rows;
        thread_data[i].barrier = &barrier;
        thread_data[i].iterations = ittr;
        thread_data[i].print_all_status = print_all_status;
        thread_data[i].debug_flag = debug_flag;
        thread_data[i].output_file = NULL;

        // Calculate block using macros
        thread_data[i].start_row = BLOCK_LOW(i, thread_count, rows-2) + 1;
        thread_data[i].end_row = BLOCK_HIGH(i, thread_count, rows-2) + 1;

        // printf("Thread %d handling rows %d to %d\n", i, thread_data[i].start_row, thread_data[i].end_row);  // Debug print

        int rc = pthread_create(&threads[i], NULL, pth_apply_stencil, (void*)&thread_data[i]);
        if(rc) {
            fprintf(stderr, "ERROR: pthread_create failed for thread %d\n", i);
            exit(1);
        }
    }

    for(int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    // Update the final array pointers
    *M_A = *(thread_data[0].A);
    *M_B = *(thread_data[0].B);

    // Clean up
    free(threads);
    free(thread_data);
}
