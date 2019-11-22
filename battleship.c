#include "battleship.h"
#include "common.h"

/* Client begins game after connecting to server */
void begin_game(int fd, int player) {
	//int round = 1;
	int ships_remaining;
	int ships_destroyed = 0;

	printf("You are player %d\n\n", player);
	ships_remaining = init_board(player);

	print_display(ships_remaining, ships_destroyed);

	while(ships_remaining > 0) {
		if(player == PLAYER_ONE) { //Player 1's loop
			send_coord(fd);
			read_coord(fd);
		} else { //Player 2's loop
			read_coord(fd);
			send_coord(fd);
		}
	}
}

/* Creates and binds file descriptor to provided port to begin listening for clients */
int open_server(char *port) {
	int listenfd;
	struct addrinfo hints, *listp, *p;

	/* Create listenfd */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;

	if(getaddrinfo(NULL, port, &hints, &listp) != 0) {
		return -1;
	}

	for(p = listp; p; p = p->ai_next) { //iterate through listp for address to bind to
		if((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
			continue; //socket failed, try next
		}

		if(bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
			break; //success
		}
		close(listenfd);
	}

	freeaddrinfo(listp);

	if(listen(listenfd, 3) < 0) {
		return -1;
	}

	return listenfd;
}

/* Client attempts to connect to server as provided host and port */
int connect_server(char *host, int port) {
	int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    if(inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0) {
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        return -1;
    }
	return sock;
}

/* Print player's board to standard output */
void print_display(int ships_remaining, int ships_destroyed) {
	char row_letter = 'A'; //starts at A

	/* Print round info */
	printf("\tShips remaining: %d\n\tShips destroyed: %d\n\n", ships_remaining, ships_destroyed);

	/* Print board */
	printf("     1    2    3    4\n\n");//columns
	for(int row = 0; row < BOARD_LENGTH; row++) {
		printf("%c    ", row_letter);
		for(int col = 0; col < BOARD_WIDTH; col++) {
			if(board[row][col] == 1) {
				printf("X    ");
			} else {
				printf("     ");
			}
		}
		printf("\n\n");
		row_letter += 1;
	}
}

/* Check board for ship at given coord, return 1 if coord is valid format, return 0 otherwise */
int validate(char *coord) {

	/*printf("\nValidating coord...\n");
	printf("coord[0] = %c\n", coord[0]);
	printf("coord[1] = %c\n", coord[1]);*/

	if((coord[0] == 'A' || coord[0] == 'B' || coord[0] == 'C' || coord[0] == 'D') &&
		(coord[1] == '1' || coord[1] == '2' || coord[1] == '3' || coord[1] == '4')) { //coord valid
		//CHECK TO MAKE SURE coord[3] IS "\n"!
		//printf("coord is valid!\n");
		return 1;

	} else {
		printf("That coordinate is invalid!\n");
		memset(&coord, 0, sizeof(coord)); //clear coord buf
		return 0;
	}
	return 0;
}

/* Randomly add ships to empty board on start of game */
int init_board(int seed) {
	srand(time(NULL) + seed); //seed random num gen
	int ships = 0;

	for(int row = 0; row < BOARD_LENGTH; row++) {
		for(int col = 0; col < BOARD_WIDTH; col++) {
			int num = rand() % 10;
			if(num <= 3 && ships != MAX_SHIPS) { 
				board[row][col] = SHIP; //place ship on board
				ships += 1;
			}
			else {
				board[row][col] = EMPTY;
			}
		}
	}

	printf("Placed %d ships on board...\n", ships);
	return ships;
}

/* Sends coord to other player */
void send_coord(int fd) {
	int valid = 0;
	while(valid == 0) { //ask for coordinate until user input is formatted correctly
		printf("Fire at coordinate: ");
		fgets(coord, sizeof(coord), stdin);
		valid = validate(coord);
		
	}
	//printf("You input: %s\n", coord);
	write(fd, &coord, sizeof(coord));
	return;
}

/* Reads and processes sent coord */
char* read_coord(int fd) {
	while(1) {
		read(fd, &coord, sizeof(coord));
		printf("Recieved %s\n", coord);
		break;
		//check = 0;
	}
	return coord;
}

/* Player fails - disconnects from server, prints fail state */
void failure() {
	return;
}

/* Player wins - disconnects from server, prints success state */
void success() {
	return;
}

/* Exit and error, print error message to standard output */
void error_exit(char *msg) {
	printf("Error: %s\n" , msg);
	exit(1);
}
