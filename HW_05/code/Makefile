CC = gcc
MPICC = mpicc
CFLAGS = -g -Wall -Wstrict-prototypes
LDFLAGS = -lm

all: nt-parallel nt-serial

nt-parallel: nt-parallel.o functions.o
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

nt-serial: nt-serial.o functions.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

nt-parallel.o: nt-parallel.c functions.h
	$(MPICC) $(CFLAGS) -c $<

nt-serial.o: nt-serial.c functions.h
	$(CC) $(CFLAGS) -c $<

functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f nt-parallel nt-serial *.o

.PHONY: all clean