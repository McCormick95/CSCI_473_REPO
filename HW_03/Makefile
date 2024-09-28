CFLAGS=-g -Wall -Wstrict-prototypes
PROGS=test-mpi-read make-2d print-2d
OBJECTS=test-mpi-read.o MyMPI.o
LDFLAGS=-lm
CC=gcc
MCC=mpicc

all: $(PROGS)

make-2d: make-2d.c
	$(CC) $(CFLAGS) -o make-2d make-2d.c

print-2d: print-2d.c
	$(CC) $(CFLAGS) -o print-2d print-2d.c

test-mpi-read: $(OBJECTS)
	$(MCC) -o test-mpi-read $(OBJECTS) $(LDFLAGS)

test-mpi-read.o: test-mpi-read.c
	$(MCC) $(CFLAGS) -c test-mpi-read.c

MyMPI.o: MyMPI.c MyMPI.h
	$(MCC) $(CFLAGS) -c MyMPI.c

clean:
	rm -f $(PROGS) *.o core*