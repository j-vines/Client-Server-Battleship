CC=gcc
CFLAGS=-O -Wall

all: bs_client bs_server

bs_client: bs_client.c common.h battleship.h battleship.c
	$(CC) $(CFLAGS) -c -o bs_client.o bs_client.c
	$(CC) $(CFLAGS) -c -o battleship.o battleship.c
	$(CC) $(LFLAGS)  -o bs_client bs_client.o battleship.o

bs_server: bs_server.c common.h battleship.h battleship.c
	$(CC) $(CFLAGS) -c -o bs_server.o bs_server.c
	$(CC) $(CFLAGS) -c -o battleship.o battleship.c
	$(CC) $(LFLAGS)  -o bs_server bs_server.o battleship.o

clean:
	rm *.o bs_client bs_server