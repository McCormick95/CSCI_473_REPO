#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    

}

void malloc2D(double ***a, int jmax, int imax){
    double **x = (double **)malloc(jmax*sizeof(double *) + jmax*imax*sizeof(double));

    x[0] = (double *)x + jmax;

    for(int j=1; j < jmax; j++){
        x[j] = x[j-1] + imax;
    }

    *a = x;
}