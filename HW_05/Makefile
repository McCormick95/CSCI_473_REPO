CFLAGS=-g -Wall -Wstrict-prototypes
PROGS=nt-parallel nt-serial functions
OBJECTS= nt-serial.o nt-parallel.o functions.o
LDFLAGS=-lm
CC=gcc
MCC=mpicc
HEADERS= functions.h

all: $(PROGS)

nt-parallel: nt-parallel.c
	$(MCC) $(CFLAGS) -o nt-parallel nt-parallel.c $(LDFLAGS)

nt-serial: nt-serial.c
	$(CC) $(CFLAGS) -o nt-serial nt-serial.c $(LDFLAGS)

functions: functions.c
	$(CC) $(CFLAGS) -o functions functions.c

functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c functions.c

clean:
	rm -f $(PROGS) *.o core*