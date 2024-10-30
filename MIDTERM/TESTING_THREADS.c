#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

typedef struct {
    double **a;          // Input array
    double **b;          // Output array
    int start_row;       // Starting row for this thread
    int end_row;         // Ending row for this thread
    int cols;            // Number of columns
    int iterations;      // Total number of iterations
    pthread_barrier_t *barrier;  // Barrier for synchronization
    void (*write_callback)(double***, int, int, int, const char*, int);  // Callback for writing output
    const char* output_file;     // Output filename
} ThreadData;

void* parallel_stencil(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    
    // For each iteration
    for(int iter = 0; iter < data->iterations; iter++) {
        // Apply stencil to this thread's portion of the array
        for(int i = data->start_row; i < data->end_row; i++) {
            for(int j = 1; j < data->cols-1; j++) {
                double sum = data->a[i-1][j-1] + data->a[i-1][j] + data->a[i-1][j+1] + 
                            data->a[i][j-1]   + data->a[i][j]   + data->a[i][j+1] + 
                            data->a[i+1][j-1] + data->a[i+1][j] + data->a[i+1][j+1];
                data->b[i][j] = sum / 9.0;
            }
        }

        // Wait for all threads to complete this iteration
        pthread_barrier_wait(data->barrier);

        // Let thread 0 handle the pointer swap and file writing
        if (data->start_row == 1) {  // Thread 0
            double **temp = data->a;
            data->a = data->b;
            data->b = temp;

            if (data->write_callback) {
                double **current = data->a;  // Use current array for writing
                data->write_callback(&current, data->end_row, data->cols, iter, data->output_file, 0);
            }
        }

        // Wait for thread 0 to complete pointer swap and file writing
        pthread_barrier_wait(data->barrier);
    }

    pthread_exit(NULL);
}

void pth_apply_stencil(double ***A, double ***B, int rows, int cols, int iterations, 
                      void (*write_callback)(double***, int, int, int, const char*, int),
                      const char* output_file) {
    double **a = *A;
    double **b = *B;
    
    // Initialize barrier for synchronization
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    
    // Create thread handles and data structures
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    
    // Calculate rows per thread
    int rows_per_thread = (rows - 2) / NUM_THREADS;  // -2 for boundary rows
    
    // Create and launch threads
    for(int t = 0; t < NUM_THREADS; t++) {
        thread_data[t].a = a;
        thread_data[t].b = b;
        thread_data[t].cols = cols;
        thread_data[t].barrier = &barrier;
        thread_data[t].iterations = iterations;
        thread_data[t].write_callback = write_callback;
        thread_data[t].output_file = output_file;
        
        // Calculate start and end rows for this thread
        thread_data[t].start_row = 1 + t * rows_per_thread;
        
        // For the last thread, make sure we process all remaining rows
        if (t == NUM_THREADS - 1) {
            thread_data[t].end_row = rows - 1;
        } else {
            thread_data[t].end_row = 1 + (t + 1) * rows_per_thread;
        }
        
        int rc = pthread_create(&threads[t], NULL, parallel_stencil, (void*)&thread_data[t]);
        if (rc) {
            printf("Error creating thread %d\n", t);
            exit(-1);
        }
    }
    
    // Wait for all threads to complete
    for(int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }
    
    // Update the input pointers to match the final state
    *A = thread_data[0].a;
    *B = thread_data[0].b;
    
    // Cleanup
    pthread_barrier_destroy(&barrier);
}

// Example usage
int main() {
    int rows = 1000;
    int cols = 1000;
    int iterations = 100;
    
    // Allocate 2D arrays
    double **A = (double**)malloc(rows * sizeof(double*));
    double **B = (double**)malloc(rows * sizeof(double*));
    for(int i = 0; i < rows; i++) {
        A[i] = (double*)malloc(cols * sizeof(double));
        B[i] = (double*)malloc(cols * sizeof(double));
    }
    
    // Initialize array A
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            A[i][j] = (double)(i + j);
            B[i][j] = 0.0;
        }
    }
    
    // Apply parallel stencil for multiple iterations
    pth_apply_stencil(&A, &B, rows, cols, iterations, NULL, NULL);
    
    // Cleanup
    for(int i = 0; i < rows; i++) {
        free(A[i]);
        free(B[i]);
    }
    free(A);
    free(B);
    
    return 0;
}