CFLAGS=-g -Wall -Wstrict-prototypes
PROGS=make-2d print-2d stencil-2d pth-stencil-2d
LDFLAGS=-lm -lpthread
CC=gcc

all: $(PROGS)

# Simple utilities
make-2d: make-2d.c
	$(CC) $(CFLAGS) -o make-2d make-2d.c 

print-2d: print-2d.c
	$(CC) $(CFLAGS) -o print-2d print-2d.c

# Serial version
stencil-2d: stencil-2d.o utilities.o
	$(CC) $(CFLAGS) -o stencil-2d stencil-2d.o utilities.o $(LDFLAGS)

stencil-2d.o: stencil-2d.c utilities.h
	$(CC) $(CFLAGS) -c stencil-2d.c

# Parallel version
pth-stencil-2d: pth-stencil-2d.o utilities_parallel.o my_barrier.o
	$(CC) $(CFLAGS) -o pth-stencil-2d pth-stencil-2d.o utilities_parallel.o my_barrier.o $(LDFLAGS)

pth-stencil-2d.o: pth-stencil-2d.c utilities.h my_barrier.h
	$(CC) $(CFLAGS) -D_PARALLEL -c pth-stencil-2d.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

utilities_parallel.o: utilities.c utilities.h my_barrier.h
	$(CC) $(CFLAGS) -D_PARALLEL -c utilities.c -o utilities_parallel.o

my_barrier.o: my_barrier.c my_barrier.h
	$(CC) $(CFLAGS) -c my_barrier.c

clean:
	rm -f $(PROGS) *.o core*