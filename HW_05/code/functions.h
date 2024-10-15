#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)

void get_user_input(int argc, char **argv, double *C, double *A, double *H, int *n, int *user_selection);

void test_neutrons( double C, double A, double H, int local_n, int *local_reflect, int *local_absorb, int *local_transmit);

#endif // FUNCTIONS_H