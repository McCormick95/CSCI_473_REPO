#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "utilities.h"
#include "my_barrier.h"
#include "timer.h"

int main(int argc, char *argv[]){
    int rows; 
    int cols;
    int ittr = -1;
    int debug_flag = -1;
    int thread_count = -1;
    double temp;
    double **A;
    double **B;
    char* f_in = NULL; // input file
    char* f_out = NULL; // output file
    char* f_all_ittr = NULL; // all-iterations

    ittr = atoi(argv[1]);
    f_in = argv[2];
    f_out = argv[3];
    debug_flag = atoi(argv[4]);
    thread_count = atoi(argv[5]);
    f_all_ittr = argv[6];

    if(argc < 5 || argc > 6 || ittr <= 0 || f_in == NULL || f_out == NULL || debug_flag < 0 || debug_flag > 2 || thread_count <= 0){
        printf("USAGE: ./stencil-2d <iterations> <input_file> <output_file> <debug_level> <num_threads> <all_stacked_file_name.raw (optional)>\n");
    }

    // read metadata
    FILE *file_in = fopen(f_in, "r");
    if(file_in == NULL){
        perror("ERROR: WHILE OPENING FILE");
        exit(1);
    }

    if(fread(&rows, sizeof(int), 1, file_in) < 0 || fread(&cols, sizeof(int), 1, file_in) < 0){
        perror("ERROR: WHILE READING METADATA");
        fclose(file_in);
        exit(1);
    }

    // validate array size
    if(rows <= 0 || cols <= 0){
        printf("ERROR: Invalid dimensions in input file\n");
        fclose(file_in);
        return 1;
    }

    malloc2D(&A, rows, cols);
    malloc2D(&B, rows, cols);

    // read array
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
           if(fread(&temp, sizeof(double), 1, file_in) != 1){
                perror("ERROR: WHILE READING DOUBLES");
                fclose(file_in);
                free(A);
                free(B);
                return 1;
            }
            A[i][j] = temp;    
        }
    }
    fclose(file_in);

    // initilize array B
    for(int i = 0; i < rows; i++){
        B[i][0] = A[i][0];
        B[i][cols-1] = A[i][cols-1];
    }

    // print initial state to file for all-iterations
    FILE *file_out_1 = fopen(f_all_ittr, "w");
    if(file_out_1 == NULL){
        perror("ERROR: WHILE OPENING ALL-ITERATIONS FILE");
        free(A);
        free(B);
        return 1;
    }
    write_file(&A, rows, cols, ittr, file_out_1, 1);

    run_stencil(&A, &B, rows, cols, ittr, thread_count, file_out_1);

    fclose(file_out_1);

    //print final snapshot
    FILE *file_out_2 = fopen(f_out, "w");
    if(file_out_2 == NULL){
        perror("ERROR: WHILE OPENING OUTPUT FILE");
        free(A);
        free(B);
        return 1;
    }
    write_file(&A, rows, cols, ittr, file_out_2, 2);
    fclose(file_out_2);

    free(A);
    free(B);
    return 0;
}

