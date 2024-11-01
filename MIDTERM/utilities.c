#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

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

// Parallel operations (only compiled when _PARALLEL is defined)
#ifdef _PARALLEL
#include <pthread.h>
#include "my_barrier.h"

void *pth_apply_stencil(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    
    for(int iter = 0; iter < data->iterations; iter++) {
        // Process assigned rows
        for(int i = data->start_row; i <= data->end_row; i++) {
            for(int j = 1; j < data->cols-1; j++) {
                double sum = data->shared->curr[i-1][j-1] + data->shared->curr[i-1][j] + data->shared->curr[i-1][j+1] + 
                            data->shared->curr[i][j-1]   + data->shared->curr[i][j]   + data->shared->curr[i][j+1]   + 
                            data->shared->curr[i+1][j-1] + data->shared->curr[i+1][j] + data->shared->curr[i+1][j+1];
                data->shared->next[i][j] = sum / 9.0;
            }
        }
        
        // First barrier: wait for all calculations to complete
        my_barrier_wait(data->barrier);
        
        // Thread 0 handles pointer swap and file output
        if (data->start_row == 1) {  // Thread 0
            // Write to file if needed
            if (data->output_file != NULL) {
                write_file(&data->shared->next, data->rows, data->cols, data->iterations, data->output_file, 0);
            }
            
            // Swap pointers
            double **temp = data->shared->curr;
            data->shared->curr = data->shared->next;
            data->shared->next = temp;
        }
        
        // Second barrier: wait for swap to complete
        my_barrier_wait(data->barrier);
    }
    
    return NULL;
}

void run_stencil(double ***A, double ***B, int rows, int cols, int ittr, int thread_count, FILE *output_file) {
    double **a = *A;
    double **b = *B;

    my_barrier_t barrier;
    my_barrier_init(&barrier, 0, thread_count);

    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    ThreadData *thread_data = malloc(thread_count * sizeof(ThreadData));
    
    // Create shared array structure
    SharedArrays *shared = malloc(sizeof(SharedArrays));
    shared->curr = a;
    shared->next = b;

    // Initialize boundaries for array B
    for(int j = 0; j < cols; j++) {
        b[0][j] = a[0][j];
        b[rows-1][j] = a[rows-1][j];
    }
    for(int i = 0; i < rows; i++) {
        b[i][0] = a[i][0];
        b[i][cols-1] = a[i][cols-1];
    }

    for(int i = 0; i < thread_count; i++) {
        thread_data[i].shared = shared;
        thread_data[i].cols = cols;
        thread_data[i].rows = rows;
        thread_data[i].barrier = &barrier;
        thread_data[i].iterations = ittr;
        thread_data[i].output_file = NULL;

        // Calculate block using macros
        thread_data[i].start_row = BLOCK_LOW(i, thread_count, rows-2) + 1;
        thread_data[i].end_row = BLOCK_HIGH(i, thread_count, rows-2) + 1;

        printf("Thread %d handling rows %d to %d\n", i, thread_data[i].start_row, thread_data[i].end_row);  // Debug print

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
    *A = shared->curr;
    *B = shared->next;

    // Clean up
    free(shared);
    free(threads);
    free(thread_data);
}
#endif // _PARALLEL