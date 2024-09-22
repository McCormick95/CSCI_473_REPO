#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

char *int_to_binary(int n, int size) {
    //calculate the number of bits needed for the string, based on the np passed to the program by the user
    int bits = (int)log2(size);
    //allocate the memory needed for the string based on the num of bits, the null terminating char and the size of a character
    char* binary_str = (char*)malloc((bits + 1)*sizeof(char)); 
    if (binary_str == NULL) {
        return NULL; 
    }

    //sets index for the character array
    int index = 0;

    //loops through the array and evaluates using a bitwise operation if there 1 oe 0 is needed and adds that to the string.
    for (int i = bits - 1; i >= 0; i--){
        if(n & (1 << i)){
            binary_str[index] = '1';
        } else {
            binary_str[index] = '0';
        }
        index++;
    }

    //sets the null terminating char and returns the string
    binary_str[index] = '\0'; 
    return binary_str;
}

void global_sum(double* result, int rank, int size, double my_value){
    //iterator to count the phase
    int iter = 0;

    //verfies that np is a power of 2 
    if ((size & (size-1)) != 0) {
		fprintf(stderr, "ERROR: SIZE IS NOT A POWER OF 2. \n");
        exit(1);
	};

    //creates a mask, checks that mask is less than size and then shifts the mask each time the loop executes
    for(int mask = 1; mask < size; mask = mask << 1){
        double my_value_2;
        //gets the new_rank by applying the mask to the current rank
        int new_rank = rank ^ mask;

        //determins which processor should be sending and recieving based on a bitwise and operation bewteen the rank and mask and checking if that is zero
        if((rank & mask) == 0){
            MPI_Ssend(&my_value, 1, MPI_DOUBLE, new_rank, 1, MPI_COMM_WORLD);
            MPI_Recv(&my_value_2, 1, MPI_DOUBLE, new_rank, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else{
            MPI_Recv(&my_value_2, 1, MPI_DOUBLE, new_rank, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Ssend(&my_value, 1, MPI_DOUBLE, new_rank, 1, MPI_COMM_WORLD);
        }

        //calls the int_to_binary and passes it the np and the ranks that need to be convered to binary strings
        char *rank_b = int_to_binary(rank, size);
        char *new_rank_b = int_to_binary(new_rank, size);

        //formated print statements
        printf(" Phase %d - P %d (%s) receiving from P %d (%s), val %.1f \n", iter, rank, rank_b, new_rank, new_rank_b, my_value_2);
        printf(" Phase %d - P %d (%s) sending   to   P %d (%s), val %.1f \n", iter, rank, rank_b, new_rank, new_rank_b, my_value);
        
        //does the actual addition to work towards computing the global sum
        my_value = my_value + my_value_2;
        iter ++;
        
        //frees the memory that was allocated in int_to_binary() for the rank char strings that were returned.
        free(rank_b);
        free(new_rank_b);
    }

    //returns the sum as result
    *result = my_value;
}


