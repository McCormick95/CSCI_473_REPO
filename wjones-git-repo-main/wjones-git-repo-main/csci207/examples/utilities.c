#include<stdio.h>
#include "utilities.h"

/*
	In this assignment, you must finish implementing all
	of these functions.  And don't add any other additional functions
 	work within the framework here 
*/

/* Allocate space for num_elements and return
	by reference
*/
void allocate_memory(int **buffer, int len) {
	if(buffer==NULL || len == 0) {
		// nothing to allocate
		return;
	}
}

/* Given the allocated space, initialize contents */
void initialize_memory(int *buffer, int len) {
	if(buffer==NULL || len == 0) {
		// nothing to initialize 
		return;
	}
}

/*Given initialized space, write contents to file, binary format*/
void write_memory_to_file(int *buffer, int len, char *fname) {
	if(buffer==NULL || len == 0) {
		// nothing to write 
		return;
	}
}

/* given the filename, determine size of file in units
	of number of integers.
*/
int determine_file_size(char *fn) {
	int num_elements = 0;
	if(fn==NULL) {
		// nothing to determine size
		return 0;
	}
	// do the work here

	return num_elements;
}


/* given an unallocated buffer, and uninitialized length, and the
	filename to work with, allocate the buffer and place the contents
	from the file into the allocated memory space.
	When this functions returns, the buffer will be allocated
	initialized, and also the length will also be knownn and returned by
	reference via the argument list
*/
void initialize_memory_from_file(int **buffer, int *len, char *fn) {
	if(!buffer||!len||!fn) {
		// can't do it
	}
	// do the work here
	
	return;
}

/* print the contents of memory to the screen
*/
void print_memory(int *buffer, int len) {
	if(!buffer || !len) {
		// nothing to do
	}
	// print the array

	return;
}

