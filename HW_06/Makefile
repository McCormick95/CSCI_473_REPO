CFLAGS=-g -Wall -Wstrict-prototypes
PROGS= make-2d print-2d stencil-2d
# OBJECTS=test-mpi-read.o functions.o
LDFLAGS=-lm
CC=gcc
# MCC=mpicc

all: $(PROGS)

make-2d: make-2d.c
	$(CC) $(CFLAGS) -o make-2d make-2d.c

print-2d: print-2d.c
	$(CC) $(CFLAGS) -o print-2d print-2d.c

stencil-2d: stencil-2d.c
	$(CC) $(CFLAGS) -o stencil-2d stencil-2d.c	

# functions: functions.c
# 	$(CC) $(CFLAGS) -o functions functions.c

# test-mpi-read: $(OBJECTS)
# 	$(MCC) -o test-mpi-read $(OBJECTS) $(LDFLAGS)

# test-mpi-read.o: test-mpi-read.c
# 	$(MCC) $(CFLAGS) -c test-mpi-read.c

# functions.o: functions.c functions.h
# 	$(MCC) $(CFLAGS) -c functions.c

clean:
	rm -f $(PROGS) *.o core*