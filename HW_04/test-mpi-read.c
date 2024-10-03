#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include "functions.h"

#define dtype double

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    if(argc!=3) {
        printf("usage: mpirun -np <p> %s <in data file> <out data file>\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }

    // void read_row_striped_matrix (
    // char *s, /* IN - File name */
    // void ***subs, /* OUT - 2D submatrix indices */
    // MPI_Datatype dtype, /* IN - Matrix element type */
    // int *m, /* OUT - Matrix rows */
    // int *n, /* OUT - Matrix cols */
    // MPI_Comm comm) /* IN - Communicator */

    // Note, only the subscripts are being passed, not the data/storage
    dtype** A; int rows, cols; 

    read_row_striped_matrix(argv[1], (void***)&A, MPI_DOUBLE, &rows, &cols, MPI_COMM_WORLD);

    // void print_row_striped_matrix (
    // void **a, /* IN - 2D array */
    // MPI_Datatype dtype, /* IN - Matrix element type */
    // int m, /* IN - Matrix rows */
    // int n, /* IN - Matrix cols */
    // MPI_Comm comm) /* IN - Communicator */

    print_row_striped_matrix((void**)A, MPI_DOUBLE, rows, cols, MPI_COMM_WORLD);

    write_row_striped_matrix(argv[2], (void**)A, MPI_DOUBLE, rows, cols, MPI_COMM_WORLD);

    my_free((void**)A);

    MPI_Finalize();
    return 0;
}
