#include "battleship.h"
#include "common.h"

/* Client begins game after connecting to server */
void begin_game(int fd, int player) {
	//int round = 1;

	clear();
	printw("You are player %d\n\n", player);
	refresh();
	sleep(WAIT);

	ships_remaining = init_board(player);

	while(ships_remaining > 0) {
		print_display(ships_remaining);
		if(player == PLAYER_ONE) { //Player 1's loop
			send_coord(fd);
			read_coord(fd);
		} else { //Player 2's loop
			read_coord(fd);
			print_display(ships_remaining);
			send_coord(fd);
		}
	}
	printw("\nPlayer %d reached end of loop.\n", player);
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
void print_display(int ships_remaining) {
	char row_letter = 'A'; //starts at A

	clear();
	/* Print round info */
	printw("\tShips remaining: %d\n\n", ships_remaining);

	/* Print board */
	printw("         1    2    3    4\n\n");//columns
	for(int row = 0; row < BOARD_LENGTH; row++) {
		printw("    %c    ", row_letter);
		for(int col = 0; col < BOARD_WIDTH; col++) {
			if(board[row][col] == 1) {
				printw("X    ");
			} else {
				printw("     ");
			}
		}
		printw("\n\n");
		row_letter += 1;
	}
	refresh();
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
		printw("That coordinate is invalid!\n");
		refresh();
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

	//printf("Placed %d ships on board...\n", ships);
	return ships;
}

/* Sends coord to other player */
void send_coord(int fd) {
	int valid = 0;
	while(valid == 0) { //ask for coordinate until user input is formatted correctly
		printw("Fire at coordinate: ");
		refresh();
		getstr(coord);
		valid = validate(coord);
		
	}
	//printf("You input: %s\n", coord);
	write(fd, &coord, sizeof(coord));
	return;
}

/* Reads and processes sent coord */
void read_coord(int fd) {
	while(1) {
		read(fd, &coord, sizeof(coord));
		if(coord[0] == 'F') { //other player reached fail state
			success(fd);
		}
		break;
	}
	check_board(coord, fd);
	return;
}

/* Checks board at recieved coord and updates board accordingly */
void check_board(char *coord, int fd) {
	int row;
	int col;

	//parse coord string for numeric coordinates
	switch(coord[0]) {
		case 'A' :
			col = 0;
			break;
		case 'B' :
			col = 1;
			break;
		case 'C' :
			col = 2;
			break;
		case 'D' :
			col = 3;
			break;
	}

	sscanf(&coord[1], "%d", &row);
	row -= 1;

	//printw("Checking board at row %d col %d\n", row, col);
	clear();
	printw("\n\nIncoming missile strike at %s!\n\n", coord);
	refresh();
	sleep(WAIT);
	//check board at coord
	if(board[col][row] != EMPTY) { //ship is hit
		clear();
		printw("\n\nYour ship has sunk!\n\n");
		board[col][row] = 0;
		ships_remaining -= 1;
	} else { //ship is not hit
		clear();
		printw("\n\nThe strike missed!\n\n");
	}
	refresh();
	sleep(WAIT);

	if(ships_remaining == 0) { //player has reached fail state
		failure(fd);
	}
	return;
}

/* Player fails - disconnects from server, prints fail state */
void failure(int fd) {
	strcpy(coord, "F");

	clear();
	printw("\n\nAll of your ships have been sunk...\n\n");
	refresh();
	sleep(WAIT);
	printw("YOU LOSE.");
	refresh();
	sleep(WAIT);
	write(fd, &coord, sizeof(coord));

	close(fd);
	endwin();
	exit(0);
}

/* Player wins - disconnects from server, prints success state */
void success(int fd) {
	clear();
	printw("\n\nYou sunk all of <other player>'s ships...\n\n");
	refresh();
	sleep(WAIT);
	printw("YOU WIN!");
	refresh();
	sleep(WAIT);

	close(fd);
	endwin();
	exit(0);
}

/* Exit and error, print error message to standard output */
void error_exit(char *msg) {
	printf("Error: %s\n" , msg);
	exit(1);
}

/* Initialize ncurses screen */
void init_curse() {
	initscr();
	cbreak();
	clear();
	refresh();
}
