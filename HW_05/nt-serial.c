#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include "functions.h"

int main(int argc, char *argv[]){
    double A = 0.0;
    double C = 0.0;
    double H = 0.0;
    int n = 0;
    
    int local_reflect, local_absorb, local_transmit;

    get_user_input(argc, argv, &C, &A, &H, &n);
    
    test_neutrons(C, A, H, n, &local_reflect, &local_absorb, &local_transmit);

    double prob_reflect = (double)local_reflect / n;
    double prob_absorb = (double)local_absorb / n;
    double prob_transmit = (double)local_transmit / n;
    printf("reflected (r) = %d, absorbed (a) = %d, transmitted (t) = %d \n", local_reflect, local_absorb, local_transmit);
    printf("r/n = %f, a/n =  %f, t/n = %f \n", prob_reflect, prob_absorb, prob_transmit);

    return 0;
}