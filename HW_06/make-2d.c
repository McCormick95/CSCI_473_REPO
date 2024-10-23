#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]){
    int rows = -1;
    int cols = -1;
    char* f_name = NULL;


    rows = atoi(argv[1]);
    cols = atoi(argv[2]);
    f_name = argv[3];

    int total = rows * cols;

    if(rows == -1 || cols == -1 || f_name == NULL){
        printf("USAGE: ./make_2d <rows> <columns> <file_name> \n");
    }

    FILE *file_out = fopen(f_name, "w");

    if(file_out == NULL){
        perror("ERROR: WHILE OPENING FILE");
        exit(0);
    }

    if(fwrite(&rows, sizeof(int), 1, file_out) < 0 || fwrite(&cols, sizeof(int), 1, file_out) < 0){
        perror("ERROR: WHILE WRITING METADATA");
        fclose(file_out);
        exit(1);
    }

    int count = 1;
    double one = 1.00;
    double zero = 0.00;

    for(int i = 0; i < total; i++){
        if(count == 1 || count == cols){
            if(fwrite(&one, sizeof(double), 1, file_out) < 0){
                perror("ERROR: WHILE WRITING DOUBLES TO FILE");
                fclose(file_out);
                exit(1);
            }
            if(count == cols){
                count = 0;
            }
        }
        else{
            if(fwrite(&zero, sizeof(double), 1, file_out) < 0){
                perror("ERROR: WHILE WRITING DOUBLES TO FILE");
                fclose(file_out);
                exit(1);
            }
        }

        count++;
    }

    // printf("STRING WRITTEN TO FILE\n");

    fclose(file_out);

    return 0;
}


    