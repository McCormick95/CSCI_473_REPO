CC=gcc
PROGS=make_data read_data
CFLAGS=-std=c99 -Wall -Wpedantic
LFLAGS=-lm

all: $(PROGS)

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

make_data.o: make_data.c utilities.h utilities.c
	$(CC) $(CFLAGS) -c make_data.c

make_data: make_data.o utilities.o
	$(CC) $(LFLAGS) -o make_data ./make_data.o utilities.o
	
read_data.o: read_data.c utilities.h utilities.c
	$(CC) $(CFLAGS) -c read_data.c

read_data: read_data.o utilities.o
	$(CC) $(LFLAGS) -o read_data ./read_data.o utilities.o
	
clean:
	rm -f *.o $(PROGS)

