#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)

/* Get the input values */
// void Get_input(int my_rank, int comm_sz, double* a_p, double* b_p,
//       int* n_p);

/* Calculate local integral  */
double Trap(int f_choice,double left_endpt, double right_endpt, int trap_count, 
   double base_len);    

/* Function we're integrating */
double f(int f_choice, double x);