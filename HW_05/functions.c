#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include "functions.h"

void get_user_input(int argc, char **argv, double *C, double *A, double *H, int *n){
    int opt;
    while ((opt = getopt(argc, argv, "C:A:H:n:")) != -1){
        switch(opt) {
            case 'C':
                *C = atof(optarg);
                assert(*C > 0.0);
                break;
            case 'A':
                *A = atof(optarg);
                assert(*A > 0.0);
                break;
            case 'H':
                *H = atof(optarg);
                assert(*H > 0.0);
                break;
            case 'n':
                *n = atoi(optarg);
                assert(*n > 0);
                break;
            default:
                fprintf(stderr, "USAGE: %s -A <double> -C <double> -H <double> -n <int> \n A must be less than C, all params. > 0\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (*A == 0.0 || *C == 0.0 || *H == 0.0 || *n == 0 || *A > *C) {
        fprintf(stderr, "ERROR: All arguments -A, -C, -H, and -n are required.\n A must be less than C, all params > 0\n");
        exit(EXIT_FAILURE);
    }
}