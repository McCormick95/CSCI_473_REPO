/************************* MACROS **************************/
#define DATA_MSG           0
#define PROMPT_MSG         1
#define RESPONSE_MSG       2

#define OPEN_FILE_ERROR    -1
#define MALLOC_ERROR       -2
#define TYPE_ERROR         -3
#define WRITE_ERROR        -4

#define MIN(a,b)           ((a)<(b)?(a):(b))
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)
#define BLOCK_OWNER(j,p,n) (((p)*((j)+1)-1)/(n))
#define PTR_SIZE           (sizeof(void*))
#define CEILING(i,j)       (((i)+(j)-1)/(j))

#define MPI_CHECK(call) \
    do { \
        int error_code = call; \
        if (error_code != MPI_SUCCESS) { \
            char error_string[MPI_MAX_ERROR_STRING]; \
            int length_of_error_string; \
            MPI_Error_string(error_code, error_string, &length_of_error_string); \
            fprintf(stderr, "MPI error in %s:%d: %s\n", __FILE__, __LINE__, error_string); \
            MPI_Abort(MPI_COMM_WORLD, error_code); \
        } \
    } while (0)

void read_row_striped_matrix (char *, void ***,
        MPI_Datatype, int *, int *, MPI_Comm);

void print_row_striped_matrix (void **, MPI_Datatype, int,
        int, MPI_Comm);

int get_size (MPI_Datatype t);

void print_submatrix (void **a, MPI_Datatype dtype, int rows, int cols);

void *my_malloc (int id, int bytes);

void my_allocate2d(int id, int local_rows, void **storage, int datum_size, int *n, void ***subs, int ptr_size);

void my_free(void **ptr);

void write_row_striped_matrix (char *file_name, void **a, MPI_Datatype dtype, int m, int n, MPI_Comm comm);

void write_submatrix (char *file_name, void **a, MPI_Datatype dtype, int rows, int cols);

void read_row_striped_matrix_halo (char *, void ***, MPI_Datatype, int *, int *, MPI_Comm);

void print_row_striped_matrix_halo (void **, MPI_Datatype, int, int, MPI_Comm);

void write_row_striped_matrix_halo (char*, void **, MPI_Datatype, int, int, MPI_Comm);
