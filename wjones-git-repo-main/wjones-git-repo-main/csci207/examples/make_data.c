#include<stdio.h>
#include<stdlib.h>
#include "utilities.h"

void usage(char **argv) {
	printf("usage: %s <num integers> <out filename> \n", argv[0]);
}

/* This program will make an array, fill it with random integer data, 
	of the specified length (from commandline) and place it in the 
	specified file, also from the commandline.

	It is not complete, you have to parse the arguments and also
	complete the referenced function calls in the utilities.c.

	You can not mofify the interfaces to those functions.  You must
	obey the prototypes and also how the functions are called below
*/

int main(int argc, char **argv) {

	if(argc!=3) {
		usage(argv);
		exit(1);
	}
	
	int *data = NULL;
	int num_elements = 0; // need to get this from user at commandline
	char *filename = NULL; // need to get this from the user at commandline

	// parse the cmdline arguments 



	// must not change anything about the calls to these functions
	allocate_memory(&data, num_elements);
	initialize_memory(data, num_elements);
	write_memory_to_file(data, num_elements, filename);
	// you can't add other calls in here.  All the main work 
	// has to be done in them.
	return 0;
}	
