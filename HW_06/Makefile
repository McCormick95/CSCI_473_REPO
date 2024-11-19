CFLAGS=-g -Wall -Wstrict-prototypes
PROGS= make-2d print-2d stencil-2d
OBJECTS= utilities.o stencil-2d.o
LDFLAGS=-lm
CC=gcc

all: $(PROGS)

make-2d: make-2d.c
	$(CC) $(CFLAGS) -o make-2d make-2d.c

print-2d: print-2d.c
	$(CC) $(CFLAGS) -o print-2d print-2d.c

stencil-2d: $(OBJECTS)
	$(CC) $(CFLAGS) -o stencil-2d $(OBJECTS)

stencil-2d.o: stencil-2d.c
	$(CC) $(CFLAGS) -c stencil-2d.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

clean:
	rm -f $(PROGS) *.o core*