#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include "functions.h"


/*
 *   Process p-1 opens a file and inputs a two-dimensional
 *   matrix, reading and distributing blocks of rows to the
 *   other processes.
 */

void read_row_striped_matrix (
   char        *s,        /* IN - File name */
   void      ***subs,     /* OUT - 2D submatrix indices */
   MPI_Datatype dtype,    /* IN - Matrix element type */
   int         *m,        /* OUT - Matrix rows */
   int         *n,        /* OUT - Matrix cols */
   MPI_Comm     comm)     /* IN - Communicator */
{
   int          datum_size;   /* Size of matrix element */
   int          i;
   int          id;           /* Process rank */
   FILE        *infileptr;    /* Input file pointer */
   int          local_rows;   /* Rows on this proc */
   int          p;            /* Number of processes */
   MPI_Status   status;       /* Result of receive */

   MPI_Comm_size (comm, &p);
   MPI_Comm_rank (comm, &id);
   datum_size = get_size (dtype);

   if (id == (p-1)) {
      infileptr = fopen (s, "r");
      if (infileptr == NULL) *m = 0;
      else {
         fread (m, sizeof(int), 1, infileptr);
         fread (n, sizeof(int), 1, infileptr);
      }      
   }
   MPI_Bcast (m, 1, MPI_INT, p-1, comm);

   if (!(*m)) MPI_Abort (MPI_COMM_WORLD, OPEN_FILE_ERROR);

   MPI_Bcast (n, 1, MPI_INT, p-1, comm);

   local_rows = BLOCK_SIZE(id,p,*m);

   if(p > *m){
        if(id == 0){
            printf("ERROR: number of processes (%d) exceeds number of rows (%d). Exiting... \n", p, *m);
        }
        MPI_Abort( comm, EXIT_FAILURE); 
   }

   void *storage;
   my_allocate2d(id, local_rows, (void **)&storage, datum_size, n, subs);

   if (id == (p-1)) {
      for (i = 0; i < p-1; i++) {
         fread (storage, datum_size,
            BLOCK_SIZE(i,p,*m) * *n, infileptr);
         MPI_Send (storage, BLOCK_SIZE(i,p,*m) * *n, dtype,
            i, DATA_MSG, comm);
      }
         fread (storage, datum_size, local_rows * *n,
         infileptr);
      fclose (infileptr);
   } else
      MPI_Recv (storage, local_rows * *n, dtype, p-1,
         DATA_MSG, comm, &status);
}


void print_row_striped_matrix (
   void **a,            /* IN - 2D array */
   MPI_Datatype dtype,  /* IN - Matrix element type */
   int m,               /* IN - Matrix rows */
   int n,               /* IN - Matrix cols */
   MPI_Comm comm)       /* IN - Communicator */
{
   MPI_Status  status;          /* Result of receive */
   void       *bstorage;        /* Elements received from
                                   another process */
   void      **b;               /* 2D array indexing into
                                   'bstorage' */
   int         datum_size;      /* Bytes per element */
   int         i;
   int         id;              /* Process rank */
   int         local_rows;      /* This proc's rows */
   int         max_block_size;  /* Most matrix rows held by
                                   any process */
   int         prompt;          /* Dummy variable */
   int         p;               /* Number of processes */

   MPI_Comm_rank (comm, &id);
   MPI_Comm_size (comm, &p);
   local_rows = BLOCK_SIZE(id,p,m);
   if (!id) {
      print_submatrix (a, dtype, local_rows, n);
      if (p > 1) {
         datum_size = get_size (dtype);
         max_block_size = BLOCK_SIZE(p-1,p,m);
         my_allocate2d(id, local_rows, (void **)&bstorage, datum_size, &n, (void ***)&b);
         b[0] = bstorage;
   
         for (i = 1; i < max_block_size; i++) {
            b[i] = b[i-1] + n * datum_size;
         }
         for (i = 1; i < p; i++) {
            MPI_Send (&prompt, 1, MPI_INT, i, PROMPT_MSG, MPI_COMM_WORLD);
            MPI_Recv (bstorage, BLOCK_SIZE(i,p,m)*n, dtype, i, RESPONSE_MSG, MPI_COMM_WORLD, &status);
            print_submatrix (b, dtype, BLOCK_SIZE(i,p,m), n);
         }
         free (b);
         free (bstorage);
      }
   } else {
      MPI_Recv (&prompt, 1, MPI_INT, 0, PROMPT_MSG, MPI_COMM_WORLD, &status);
      MPI_Send (*a, local_rows * n, dtype, 0, RESPONSE_MSG, MPI_COMM_WORLD);
   }
}

void write_row_striped_matrix (
   char *file_name,
   void **a,            /* IN - 2D array */
   MPI_Datatype dtype,  /* IN - Matrix element type */
   int m,               /* IN - Matrix rows */
   int n,               /* IN - Matrix cols */
   MPI_Comm comm)       /* IN - Communicator */
{
   MPI_Status  status;          /* Result of receive */
   void       *bstorage;        /* Elements received from another process */
   void      **b;               /* 2D array indexing into 'bstorage' */
   int         datum_size;      /* Bytes per element */
   int         i;
   int         id;              /* Process rank */
   int         local_rows;      /* This proc's rows */
   int         max_block_size;  /* Most matrix rows held by any process */
   int         prompt;          /* Dummy variable */
   int         p;               /* Number of processes */

   MPI_Comm_rank (comm, &id);
   MPI_Comm_size (comm, &p);
   local_rows = BLOCK_SIZE(id,p,m);

   FILE *outfileptr;

   if (id == 0) {
      outfileptr = fopen (file_name, "w");
      if (outfileptr == NULL) m = 0;
      else {
         fwrite (&m, sizeof(int), 1, outfileptr);
         fwrite (&n, sizeof(int), 1, outfileptr);
      }

      fclose (outfileptr);      
   }
   
   MPI_Barrier(comm);

   if (!id) {
      write_submatrix ( file_name, a, dtype, local_rows, n);
      if (p > 1) {
         datum_size = get_size (dtype);
         max_block_size = BLOCK_SIZE(p-1,p,m);
         my_allocate2d(id, local_rows, (void **)&bstorage, datum_size, &n, (void ***)&b);
         b[0] = bstorage;
         for (i = 1; i < max_block_size; i++) {
            b[i] = b[i-1] + n * datum_size;
         }
         for (i = 1; i < p; i++) {
            MPI_Send (&prompt, 1, MPI_INT, i, PROMPT_MSG, MPI_COMM_WORLD);
            MPI_Recv (bstorage, BLOCK_SIZE(i,p,m)*n, dtype, i, RESPONSE_MSG, MPI_COMM_WORLD, &status);
            write_submatrix ( file_name, a, dtype, BLOCK_SIZE(i,p,m), n);
         }
         free (b);
         free (bstorage);
      }
   } else {
      MPI_Recv (&prompt, 1, MPI_INT, 0, PROMPT_MSG, MPI_COMM_WORLD, &status);
      MPI_Send (*a, local_rows * n, dtype, 0, RESPONSE_MSG, MPI_COMM_WORLD);
   }
}

void write_submatrix (
   char *file_name,
   void       **a,       /* OUT - Doubly-subscripted array */
   MPI_Datatype dtype,   /* OUT - Type of array elements */
   int          rows,    /* OUT - Matrix rows */
   int          cols)    /* OUT - Matrix cols */    
{
   FILE *file_out; 
   file_out = fopen (file_name, "a");
   int i, j;

   for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++) {
         if (dtype == MPI_DOUBLE){
            fwrite(&((double **)a)[i][j], sizeof(double), 1, file_out);
         }
         else {
            if (dtype == MPI_FLOAT){
               fwrite(&((float **)a)[i][j], sizeof(float), 1, file_out);
            }   
            else if (dtype == MPI_INT){
               fwrite(&((int **)a)[i][j], sizeof(int), 1, file_out);
            }   
         }
      }
   }
   fclose(file_out);
}

/*
 *   Print elements of a doubly-subscripted array.
 */

void print_submatrix (
   void       **a,       /* OUT - Doubly-subscripted array */
   MPI_Datatype dtype,   /* OUT - Type of array elements */
   int          rows,    /* OUT - Matrix rows */
   int          cols)    /* OUT - Matrix cols */
{
   int i, j;

   for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++) {
         if (dtype == MPI_DOUBLE)
            printf ("%6.3f ", ((double **)a)[i][j]);
         else {
            if (dtype == MPI_FLOAT)
               printf ("%6.3f ", ((float **)a)[i][j]);
            else if (dtype == MPI_INT)
               printf ("%6d ", ((int **)a)[i][j]);
         }
      }
      putchar ('\n');
   }
}

/*
 *   Given MPI_Datatype 't', function 'get_size' returns the
 *   size of a single datum of that data type.
 */

int get_size (MPI_Datatype t) {
   if (t == MPI_BYTE) return sizeof(char);
   if (t == MPI_DOUBLE) return sizeof(double);
   if (t == MPI_FLOAT) return sizeof(float);
   if (t == MPI_INT) return sizeof(int);
   printf ("Error: Unrecognized argument to 'get_size'\n");
   fflush (stdout);
   MPI_Abort (MPI_COMM_WORLD, TYPE_ERROR);

   return -1;
}

/*
 *   Function 'my_malloc' is called when a process wants
 *   to allocate some space from the heap. If the memory
 *   allocation fails, the process prints an error message
 *   and then aborts execution of the program.
 */

void *my_malloc (
   int id,     /* IN - Process rank */
   int bytes)  /* IN - Bytes to allocate */
{
   void *buffer;
   if ((buffer = malloc ((size_t) bytes)) == NULL) {
      printf ("Error: Malloc failed for process %d\n", id);
      fflush (stdout);
      MPI_Abort (MPI_COMM_WORLD, MALLOC_ERROR);
   }
   return buffer;
}

void my_allocate2d(int id, int local_rows, void **storage, int datum_size, int *n, void ***subs){
   void **lptr;
   void *rptr;
   *storage = (void *) my_malloc (id, local_rows * *n * datum_size);
   *subs = (void **) my_malloc (id, local_rows * PTR_SIZE);

   lptr = (void *) &(*subs[0]);
   rptr = (void *) *storage;
   int i;
   for (i = 0; i < local_rows; i++) {
      *(lptr++)= (void *) rptr;
      rptr += *n * datum_size;
   }
}

void my_free(void **ptr) {
   free(ptr[0]);
   free(ptr);
}

