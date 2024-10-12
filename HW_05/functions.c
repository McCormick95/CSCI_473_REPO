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

void test_neutrons( double C, double A, double H, int local_n, int *local_reflect, int *local_absorb, int *local_transmit){
    double L_dist = 0.0;
    double u_Rnum = 0.0;
    double direction = 0.0;
    double x_position = 0.0;
    bool bounce_status = true;

    *local_absorb = 0;
    *local_reflect = 0;
    *local_transmit = 0;

    // unsigned short seed[3];
    // time_t t;
    // time(&t);
    // seed[0] = (unsigned short)t;
    // seed[1] = (unsigned short)(t >> 16);
    // seed[2] = (unsigned short)(t >> 32);
    unsigned short seed[3] ={0,0,0};

    for(int i = 0; i < local_n; i++){
        direction = 0.0;
        x_position = 0.0;
        bounce_status = true;
        while(bounce_status == true){
            u_Rnum = erand48(seed);
            L_dist = -(1/C) * log(u_Rnum);
            x_position += (L_dist * cos(direction));
            if(x_position < 0){
                (*local_reflect)++;
                bounce_status = false;
            }
            else if(x_position >= H){
                (*local_transmit)++;
                bounce_status = false;
            }
            else if(u_Rnum < (A/C)){
                (*local_absorb)++;
                bounce_status = false;
            }
            else{
                direction = u_Rnum * M_PI;
            }
        }
    }
}