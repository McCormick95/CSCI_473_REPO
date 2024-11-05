CFLAGS=-g -Wall -Wstrict-prototypes
PROGS=make-2d print-2d stencil-2d pth-stencil-2d omp-stencil-2d
LDFLAGS=-lm 
OMPFLAG=-fopenmp
PTHFLAG=-lpthread
CC=gcc

all: $(PROGS)

# Simple utilities
make-2d: make-2d.c
	$(CC) $(CFLAGS) -o make-2d make-2d.c $(LDFLAGS)

print-2d: print-2d.c
	$(CC) $(CFLAGS) -o print-2d print-2d.c $(LDFLAGS)

# Serial version
stencil-2d: stencil-2d.o utilities.o my_barrier.o
	$(CC) $(CFLAGS) -o stencil-2d stencil-2d.o utilities.o my_barrier.o $(LDFLAGS) $(PTHFLAG) $(OMPFLAG)

stencil-2d.o: stencil-2d.c utilities.h
	$(CC) $(CFLAGS) -c stencil-2d.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) $(OMPFLAG) -c utilities.c

my_barrier.o: my_barrier.c my_barrier.h
	$(CC) $(CFLAGS) -c my_barrier.c

# Pthread version
pth-stencil-2d: pth-stencil-2d.o utilities.o my_barrier.o
	$(CC) $(CFLAGS) -o pth-stencil-2d pth-stencil-2d.o utilities.o my_barrier.o $(LDFLAGS) $(PTHFLAG) $(OMPFLAG)

pth-stencil-2d.o: pth-stencil-2d.c utilities.h my_barrier.h
	$(CC) $(CFLAGS) -c pth-stencil-2d.c

# OpenMP version
omp-stencil-2d: omp-stencil-2d.o utilities.o my_barrier.o
	$(CC) $(CFLAGS) -o omp-stencil-2d omp-stencil-2d.o utilities.o my_barrier.o $(LDFLAGS) $(PTHFLAG) $(OMPFLAG)

omp-stencil-2d.o: omp-stencil-2d.c utilities.h
	$(CC) $(CFLAGS) $(OMPFLAG) -c omp-stencil-2d.c

clean:
	rm -f $(PROGS) *.o core*