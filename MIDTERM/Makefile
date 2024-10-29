CFLAGS=-g -Wall -Wstrict-prototypes
PROGS= make-2d print-2d stencil-2d omp-stencil-2d pth-stencil-2d
OBJECTS= utilities.o stencil-2d.o my_barrier.o pth-stencil-2d.o omp-stencil-2d.o
LDFLAGS=-lm -l -pthread
CC=gcc

# $(LDFLAGS)

all: $(PROGS)

make-2d: make-2d.c
	$(CC) $(CFLAGS) -o make-2d make-2d.c 

print-2d: print-2d.c
	$(CC) $(CFLAGS) -o print-2d print-2d.c

stencil-2d: $(OBJECTS)
	$(CC) $(CFLAGS) -o stencil-2d $(OBJECTS)

stencil-2d.o: stencil-2d.c
	$(CC) $(CFLAGS) -c stencil-2d.c

pth-stencil-2d: $(OBJECTS)
	$(CC) $(CFLAGS) -o pth-stencil-2d $(OBJECTS)

pth-stencil-2d.o: pth-stencil-2d.c
	$(CC) $(CFLAGS) -c pth-stencil-2d.c

omp-stencil-2d: $(OBJECTS)
	$(CC) $(CFLAGS) -o omp-stencil-2d $(OBJECTS)

omp-stencil-2d.o: omp-stencil-2d.c
	$(CC) $(CFLAGS) -c omp-stencil-2d.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

my_barrier.o: my_barrier.c my_barrier.h
	$(CC) $(CFLAGS) -c my_barrier.c

clean:
	rm -f $(PROGS) *.o core*