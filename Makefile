CC=gcc
CFLAGS=-O -Wall
LFLAGS=-lpthread -lcurses

all: battleship

battleship: launcher.c menu.h common.h battleship.c battleship.h
	$(CC) $(CFLAGS) -c -o launcher.o launcher.c
	$(CC) $(CFLAGS) -c -o battleship.o battleship.c
	$(CC) -o battleship launcher.o battleship.o $(LFLAGS)
	$(CC) rm *.o

clean:
	rm -f *.o prev_ip.txt battleship

test:
	./battleship