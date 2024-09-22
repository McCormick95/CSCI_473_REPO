/* Allocate space for num_elements and return
	by reference
*/
void allocate_memory(int **buffer, int len);

/* Given the allocated space, initialize contents */
void initialize_memory(int *buffer, int len);

/*Given initialized space, write contents to file, binary format*/
void write_memory_to_file(int *buffer, int len, char *fname);

/* given an unallocated buffer, and uninitialized length, and the
	filename to work with, allocate the buffer and place the contents
	from the file into the allocated memory space.
	When this functions returns, the buffer will be allocated
	initialized, and also the length will also be knownn and returned by
	reference via the argument list
*/
void initialize_memory_from_file(int **buffer, int *len, char *fn);

/* print the contents of memory to the screen
*/
void print_memory(int *buffer, int len);
