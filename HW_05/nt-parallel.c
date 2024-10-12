#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>
#include "functions.h"

int main(int argc, char *argv[]){
    MPI_Init(NULL, NULL);
    double A = 0.0;
    double C = 0.0;
    double H = 0.0;
    int n = 0;

    get_user_input(argc, argv, &C, &A, &H, &n);

    int local_reflect, local_absorb, local_transmit;
    int total_reflect, total_absorb, total_transmit;
    int np, id, local_n;

    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &np);

    local_n = BLOCK_SIZE(id, np, n);

    test_neutrons(C, A, H, local_n, &local_reflect, &local_absorb, &local_transmit);

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

