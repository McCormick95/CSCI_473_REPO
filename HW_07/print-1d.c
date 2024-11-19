#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int rows; 
    double temp;

    char* f_name = NULL;
    f_name = argv[1];

    if(f_name == NULL){
        printf("USAGE: ./print_2d <file_name> \n");
    }

    FILE *file_in = fopen(f_name, "r");

    if(file_in == NULL){
        perror("ERROR: WHILE OPENING FILE");
        exit(0);
    }

    //read meta data
    if(fread(&rows, sizeof(int), 1, file_in) < 0){
        perror("ERROR: WHILE READING METADATA");
        fclose(file_in);
        exit(0);
    }

    //print array based on size given in mata data
    for(int i = 0; i < rows; i++){
        if(fread(&temp, sizeof(double), 1, file_in) < 0){
            perror("ERROR: WHILE READING DOUBLES");
            fclose(file_in);
            exit(0);
        }
        printf("%6.2f ", temp); 
        printf("\n");
    }

    fclose(file_in);

    return 0;
}

