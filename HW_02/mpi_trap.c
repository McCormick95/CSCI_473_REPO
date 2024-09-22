/* File:     mpi_trap3.c
 * Purpose:  Use MPI to implement a parallel version of the trapezoidal 
 *           rule.  This version uses collective communications to 
 *           distribute the input data and compute the global sum.
 *
 * Input:    The endpoints of the interval of integration and the number
 *           of trapezoids
 * Output:   Estimate of the integral from a to b of f(x)
 *           using the trapezoidal rule and n trapezoids.
 *
 * Compile:  mpicc -g -Wall -o mpi_trap2 mpi_trap2.c
 * Run:      mpiexec -n <number of processes> ./mpi_trap2
 *
 * Algorithm:
 *    1.  Each process calculates "its" interval of
 *        integration.
 *    2.  Each process estimates the integral of f(x)
 *        over its interval using the trapezoidal rule.
 *    3a. Each process != 0 sends its integral to 0.
 *    3b. Process 0 sums the calculations received from
 *        the individual processes and prints the result.
 *
 * Notes:  
 *    1. f(x) is all hardwired.
 *    2. The number of trapezoids should be evenly divisible by
 *       the number of processes.
 *
 * IPP2:  Section 3.4.2 (pp. 110 and ff.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>
#include "trap.h"

int main(int argc, char *argv[]) {
   int opt;
   long int n;
   int my_rank, comm_sz, local_n, f_choice;   
   double a, b, h, local_a, local_b;
   double local_int, total_int;
   double start_time, end_time, elapsed_time;
   int n_set = 0, a_set = 0, b_set = 0, f_set = 0;

   /* Let the system do what it needs to start up MPI */
   MPI_Init(NULL, NULL);

   while ((opt = getopt(argc, argv, "n:a:b:f:")) != -1) {
      switch (opt) {
         case 'n':
               n = strtol(optarg, NULL, 10);
               n_set = 1;
               break;
         case 'a':
               a = strtod(optarg, NULL);
               a_set = 1;
               break;
         case 'b':
               b = strtod(optarg, NULL);
               b_set = 1;
               break;
         case 'f':
               f_choice = atoi(optarg);
               f_set = 1;
               break;
         case '?':
               fprintf(stderr, "Usage: %s -n <num_of_divisions> -a <lower_bound> -b <upper_bound> -f <funct_to_integrate>\n", argv[0]);
               exit(EXIT_FAILURE);
         default:
               fprintf(stderr, "Usage: %s -n <num_of_divisions> -a <lower_bound> -b <upper_bound> -f <funct_to_integrate>\n", argv[0]);
               exit(EXIT_FAILURE);
      }
   }

   if (!n_set || !a_set || !b_set || !f_set || f_choice >= 4 || f_choice <= 0 || a >= b || n <= 0) {
      fprintf(stderr, "Usage: %s -n <num_of_divisions> -a <lower_bound> -b <upper_bound> -f <funct_to_integrate>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   /* Get my process rank */
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   /* Find out how many processes are being used */
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

   // Start the timer
   start_time = MPI_Wtime();

   // Get_input(my_rank, comm_sz, &a, &b, &n);

   h = (b-a)/n;          /* h is the same for all processes */
   //local_n = n/comm_sz;  /* So is the number of trapezoids  */
   local_n = BLOCK_SIZE(my_rank, comm_sz, n);
   /* Length of each process' interval of
    * integration = local_n*h.  So my interval
    * starts at: */
   local_a = a + BLOCK_LOW(my_rank, comm_sz, n) * h;
   local_b = local_a + local_n*h;
   local_int = Trap(f_choice, local_a, local_b, local_n, h);

   printf("rank = %d, local_a= %.2f, local_b= %.2f, local_int= %.2f \n", my_rank, local_a, local_b, local_int);

   /* Add up the integrals calculated by each process */
   MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0,
         MPI_COMM_WORLD);

   /* Print the result */
   if (my_rank == 0) {
      printf("n = %ld \n", n);
      //printf("of the integral from %f to %f = %.15e\n", a, b, total_int);
   }

   // End the timer
   end_time = MPI_Wtime();

   // Calculate the elapsed time
   elapsed_time = end_time - start_time;

   // Print the elapsed time
   if (my_rank == 0) {
      printf("T: %f \n", elapsed_time);
   }

   /* Shut down MPI */
   MPI_Finalize();

   return 0;
} /*  main  */


