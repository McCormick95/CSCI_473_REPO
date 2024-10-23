#include <stdio.h>
#include <stdlib.h>

void malloc2D(double ***a, int jmax, int imax);
void apply_stencil(double ***A, double ***B, int rows, int cols);
void write_file(double ***A, int rows, int cols, int ittr, FILE *f_name, int flag);

int main(int argc, char *argv[]){
    int rows; 
    int cols;
    int ittr = 0;
    double temp;
    double **A;
    double **B;
    char* f_in = NULL; // input file
    char* f_out = NULL; // output file
    char* f_all_ittr = NULL; // all-iterations

    ittr = atoi(argv[1]);
    f_in = argv[2];
    f_out = argv[3];
    f_all_ittr = argv[4];


    if(argc != 5 || ittr <= 0 || f_in == NULL || f_out == NULL || f_all_ittr == NULL){
        printf("USAGE: ./stencil-2d <iterations> <input_file> <output_file> <all_iterations>\n");
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

    printf("----------TESTING----------\n");
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            printf("%8.2f ", A[i][j]);
        }
        printf("\n");
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

    for(int i = 0; i < ittr; i++){
        apply_stencil(&A, &B, rows, cols);
        
        double **temp_ptr = A;
        A = B;
        B = temp_ptr;

        write_file(&A, rows, cols, ittr, file_out_1, 0);
    }
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

void malloc2D(double ***a, int jmax, int imax){
    double **x = (double **)malloc(jmax*sizeof(double *) + jmax*imax*sizeof(double));

    x[0] = (double *)x + jmax;

    for(int j=1; j < jmax; j++){
        x[j] = x[j-1] + imax;
    }

    *a = x;
}

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