#ifndef UTILITIES_H
#define UTILITIES_H

void malloc2D(double ***a, int jmax, int imax);
void apply_stencil(double ***A, double ***B, int rows, int cols);
void write_file(double ***A, int rows, int cols, int ittr, FILE *f_name, int flag);

#endif // UTILITIES_H