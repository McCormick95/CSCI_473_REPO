#include<stdio.h>
#include<stdlib.h>
#include "utilities.h"

void usage(char **argv) {
	printf("usage: %s <out filename> \n", argv[0]);
}

/* This program fill an array with the contents from the specified file.

	It is not complete, you have to parse the arguments and also
	complete the referenced function calls in the utilities.c.

	You can not mofify the interfaces to those functions.  You must
	obey the prototypes and also how the functions are called below
*/

int main(int argc, char **argv) {

	if(argc!=2) {
		usage(argv);
		exit(1);
	}
	
	int *data = NULL;
	int num_elements = 0; 
	char *filename = NULL;

	// parse the cmdline inputs


	// must not change these calls to these functions 
	initialize_memory_from_file(&data, &num_elements, filename);
	print_memory(data, num_elements);
	// and the main work has to be done in them
	return 0;
}	
