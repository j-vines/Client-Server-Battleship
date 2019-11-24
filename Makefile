CC=gcc
CFLAGS=-O -Wall
LFLAGS=-pthread -lcurses

all: bs_join bs_host

bs_join: bs_client.c common.h battleship.h battleship.c
	$(CC) $(CFLAGS) -c -o bs_client.o bs_client.c
	$(CC) $(CFLAGS) -c -o battleship.o battleship.c
	$(CC) -o bs_join bs_client.o battleship.o $(LFLAGS)

bs_host: bs_server.c common.h battleship.h battleship.c
	$(CC) $(CFLAGS) -c -o bs_server.o bs_server.c
	$(CC) $(CFLAGS) -c -o battleship.o battleship.c
	$(CC) -o bs_host bs_server.o battleship.o $(LFLAGS)

clean:
	rm *.o bs_join bs_host

test:
	./bs_host 80