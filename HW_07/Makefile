CFLAGS=-g -fopenmp -Wall -Wstrict-prototypes
PROGS= make-2d print-2d omp-matrix-vector print-1d
OBJECTS= utilities.o omp-matrix-vector.o
LDFLAGS=-lm
CC=gcc

all: $(PROGS)

make-2d: make-2d.c
	$(CC) $(CFLAGS) -o make-2d make-2d.c

print-2d: print-2d.c
	$(CC) $(CFLAGS) -o print-2d print-2d.c

omp-matrix-vector: $(OBJECTS)
	$(CC) $(CFLAGS) -o omp-matrix-vector $(OBJECTS)

print-1d: print-1d.c
	$(CC) $(CFLAGS) -o print-1d print-1d.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

clean:
	rm -f $(PROGS) *.o core*