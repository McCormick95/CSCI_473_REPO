#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include "functions.h"

int main(int argc, char *argv[]){
    double A = 0.0;
    double C = 0.0;
    double H = 0.0;
    int n = 0;
    int local_reflect, local_absorb, local_transmit;
    struct timespec start, end;
    double elapsed_time;
    
    get_user_input(argc, argv, &C, &A, &H, &n);
    
    // Start the timer
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    test_neutrons(C, A, H, n, &local_reflect, &local_absorb, &local_transmit);
    
    // End the timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    double prob_reflect = (double)local_reflect / n;
    double prob_absorb = (double)local_absorb / n;
    double prob_transmit = (double)local_transmit / n;
    printf("reflected (r) = %d, absorbed (a) = %d, transmitted (t) = %d \n", local_reflect, local_absorb, local_transmit);
    printf("r/n = %f, a/n =  %f, t/n = %f \n", prob_reflect, prob_absorb, prob_transmit);

    elapsed_time = (end.tv_sec - start.tv_sec) +
                (end.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("T: %f \n", elapsed_time);

    return 0;
}