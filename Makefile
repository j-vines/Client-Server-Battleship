CC=gcc
CFLAGS=-O -Wall
LFLAGS=-lpthread -lcurses

all: battleship

battleship: bs.c menu.h common.h battleship.c battleship.h
	$(CC) $(CFLAGS) -c -o bs.o bs.c
	$(CC) $(CFLAGS) -c -o battleship.o battleship.c
	$(CC) -o battleship bs.o battleship.o $(LFLAGS)

clean:
	rm *.o prev_ip.txt battleship

test:
	./battleship