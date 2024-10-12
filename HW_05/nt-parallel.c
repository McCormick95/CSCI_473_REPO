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
    int user_selection = 0;
    double start_time, end_time, elapsed_time;

    get_user_input(argc, argv, &C, &A, &H, &n, &user_selection);

    int local_reflect, local_absorb, local_transmit;
    int total_reflect, total_absorb, total_transmit;
    int np, id, local_n;

    // Start the timer
    start_time = MPI_Wtime();
    
    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &np);

    local_n = BLOCK_SIZE(id, np, n);

    test_neutrons(C, A, H, local_n, &local_reflect, &local_absorb, &local_transmit);

    MPI_Reduce(&local_reflect, &total_reflect, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_absorb, &total_absorb, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_transmit, &total_transmit, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // End the timer
    end_time = MPI_Wtime();
    elapsed_time = end_time - start_time;

    if(id == 0){
        double prob_reflect = (double)total_reflect / n;
        double prob_absorb = (double)total_absorb / n;
        double prob_transmit = (double)total_transmit / n;

        if(user_selection == 0){
            printf("reflected (r) = %d, absorbed (a) = %d, transmitted (t) = %d \n", total_reflect, total_absorb, total_transmit);
            printf("r/n = %f, a/n =  %f, t/n = %f \n", prob_reflect, prob_absorb, prob_transmit);
            printf("T: %f \n", elapsed_time);
        }
        else{
            printf("time,refl,abs,tran,rn,an,tn,\n");
            printf("%f,%d,%d,%d,%f,%f,%f,\n", elapsed_time, total_reflect, total_absorb, total_transmit, prob_reflect, prob_absorb, prob_transmit);
        }
        
    }

    MPI_Finalize();
    return 0;
}

