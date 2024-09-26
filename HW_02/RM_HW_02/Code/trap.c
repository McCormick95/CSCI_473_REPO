#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "trap.h"

double Trap(
      int f_choice,
      double left_endpt, 
      double right_endpt, 
      int    trap_count, 
      double base_len) {
   double estimate, x; 
   int i;

   estimate = (f(f_choice, left_endpt) + f(f_choice, right_endpt))/2.0;
   for (i = 1; i <= trap_count-1; i++) {
      x = left_endpt + i*base_len;
      estimate += f(f_choice, x);
   }
   estimate = estimate*base_len;

   return estimate;
} /*  Trap  */


double f(int f_choice, double x) {
   if(f_choice == 1){
      return x;
   }
   else if(f_choice == 2){
      return x*x;
   }
   else{
      return sin(x) * (180.0/M_PI);
   }
} /* f */


