#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include "timer.h"

int main(int argc, char *argv[]){
    int rows; 
    int cols;
    int ittr = 0;
    int debug_flag = -1;
    int print_all_status = 0;
    double start_time;
    double end_time;
    double start_work_time;
    double end_work_time;
    double work_time_total;
    double total_time;
    double other_time;
    double temp;
    double **A;
    double **B;
    char* f_in = NULL; // input file
    char* f_out = NULL; // output file
    char* f_all_ittr = NULL; // all-iterations

    GET_TIME(start_time);

    ittr = atoi(argv[1]);
    f_in = argv[2];
    f_out = argv[3];
    debug_flag = atoi(argv[4]);

    if(argv[5] != NULL){
        f_all_ittr = argv[5];
    }

    if(argc < 4 || argc > 5 || ittr <= 0 || f_in == NULL || f_out == NULL || debug_flag < 0 || debug_flag > 2 ){
        printf("USAGE: ./stencil-2d <iterations> <input_file> <output_file> <debug_level> <all_stacked_file_name.raw (optional)>\n");
        exit(1);
    }

    if(f_all_ittr != NULL){
        print_all_status = 1;
    }

    FILE *file_in = fopen(f_in, "r");
    if(file_in == NULL){
        perror("ERROR: WHILE OPENING FILE");
        exit(1);
    }
    // read metadata
    if(fread(&rows, sizeof(int), 1, file_in) < 0 || fread(&cols, sizeof(int), 1, file_in) < 0){
        perror("ERROR: WHILE READING METADATA");
        fclose(file_in);
        exit(1);
    }

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

    for(int i = 0; i < rows; i++){
        B[i][0] = A[i][0];
        B[i][cols-1] = A[i][cols-1];
    }

    FILE *file_out_1 = NULL;
    if(print_all_status == 1){
        // print initial state to file for all-iterations
        file_out_1 = fopen(f_all_ittr, "w");
        if(file_out_1 == NULL){
            perror("ERROR: WHILE OPENING ALL-ITERATIONS FILE");
            free(A);
            free(B);
            return 1;
        }
        write_file(&A, rows, cols, ittr, file_out_1, 1);
    }

    GET_TIME(start_work_time);

    // APPLY STENCIL 
    for(int i = 0; i < ittr; i++){
        apply_stencil_serial(&A, &B, rows, cols);
        
        double **temp_ptr = A;
        A = B;
        B = temp_ptr;

        if(print_all_status == 1){
            write_file(&A, rows, cols, ittr, file_out_1, 0);
        }
    }

    GET_TIME(end_work_time);

    if(print_all_status == 1){
        fclose(file_out_1);
    }

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

    GET_TIME(end_time);

    work_time_total = end_work_time - start_work_time;
    total_time = end_time - start_time;
    other_time = total_time - work_time_total;

    printf("SERIAL- TOTAL: %f, WORK: %f, OTHER: %f \n", total_time, work_time_total, other_time);

    return 0;
}