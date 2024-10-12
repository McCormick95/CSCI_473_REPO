#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>
#include "functions.h"

#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)

void test_neutrons( double C, double A, double H,
    int local_n, int *local_reflect, int *local_absorb, int *local_transmit);   

int main(int argc, char *argv[]){
    MPI_Init(NULL, NULL);
    int opt;
    double A = 0.0;
    double C = 0.0;
    double H = 0.0;
    int n = 0;

    while ((opt = getopt(argc, argv, "C:A:H:n:")) != -1){
        switch(opt) {
            case 'C':
                C = atof(optarg);
                assert(C > 0.0);
                break;
            case 'A':
                A = atof(optarg);
                assert(A > 0.0);
                break;
            case 'H':
                H = atof(optarg);
                assert(H > 0.0);
                break;
            case 'n':
                n = atoi(optarg);
                assert(n > 0);
                break;
            default:
                fprintf(stderr, "USAGE: %s -A <double> -C <double> -H <double> -n <int> \n A must be less than C, all params. > 0\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (A == 0.0 || C == 0.0 || H == 0.0 || n == 0 || A > C) {
        fprintf(stderr, "ERROR: All arguments -A, -C, -H, and -n are required.\n A must be less than C, all params > 0\n");
        exit(EXIT_FAILURE);
    }

    int local_reflect, local_absorb, local_transmit;
    int total_reflect, total_absorb, total_transmit;
    int np, id, local_n;

    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &np);

    local_n = BLOCK_SIZE(id, np, n);

    test_neutrons(C, A, H, local_n, &local_reflect, &local_absorb, &local_transmit);

    printf("TESTING: id = %d, l_n= %d, l_r= %d, l_a= %d, l_t= %d \n", id, local_n, local_reflect, local_absorb, local_transmit);

    MPI_Reduce(&local_reflect, &total_reflect, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_absorb, &total_absorb, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_transmit, &total_transmit, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(id == 0){
        double prob_reflect = (double)total_reflect / n;
        double prob_absorb = (double)total_absorb / n;
        double prob_transmit = (double)total_transmit / n;
        printf("reflected (r) = %d, absorbed (a) = %d, transmitted (t) = %d \n", total_reflect, total_absorb, total_transmit);
        printf("r/n = %f, a/n =  %f, t/n = %f \n", prob_reflect, prob_absorb, prob_transmit);
    }
    
    MPI_Finalize();
    return 0;
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

    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

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
    printf("id= %d, r= %d, t= %d, a= %d \n", id, *local_reflect, *local_transmit, *local_absorb);
}