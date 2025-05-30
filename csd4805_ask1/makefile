CC=gcc
CFLAGS=-I.

all: main.o io.o logic.o
	$(CC) -o hy345sh *.o

main.o: main.c defs.h
	$(CC) -c -o main.o main.c

io.o: io.c io.h defs.h
	$(CC) -c -o io.o io.c

logic.o: logic.c logic.h defs.h
	$(CC) -c -o logic.o logic.c

clean:
	rm -f *.o hy345sh