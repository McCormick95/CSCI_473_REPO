#ifndef UTILITIES_H
#define UTILITIES_H

#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)

void malloc2D(double ***a, int jmax, int imax);
void apply_stencil(double ***A, double ***B, int rows, int cols);
void write_file(double ***A, int rows, int cols, int ittr, FILE *f_name, int flag);

#endif // UTILITIES_H