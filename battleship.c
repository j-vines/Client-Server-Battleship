#include "battleship.h"
#include "common.h"

pthread_mutex_t lock;

/* Client begins game after connecting to server */
void begin_game(int *fd, int player) {
	pthread_t read_id;
	pthread_t write_id;

	if(player == PLAYER_ONE) {
		other_player = PLAYER_TWO;
	} else {
		other_player = PLAYER_ONE;
	}

	clear();
	printw("You are Player %d\n\n", player);
	refresh();
	sleep(WAIT);

	//player places their boards
	ships_remaining = init_board(player); 
	ships_destroyed = 0;

	//display game start screen
	start_screen();

	//create reading and writing processes
	pthread_create(&read_id, 0, read_data, (void *)fd);
	pthread_create(&write_id, 0, write_data, (void *)fd);

	//main game loop
	while(ships_remaining > 0) {
		
		print_display();
		
		if(player == PLAYER_ONE) { //Player 1's loop
			send_coord(*fd);
			read_coord(*fd);
		} else { //Player 2's loop
			read_coord(*fd);
			send_coord(*fd);
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
void print_display() {
	char row_letter = 'A'; //starts at A

	clear();
	/* Print round info */
	printw("\tShips remaining: %d\n", ships_remaining);
	printw("\tShips destroyed: %d\n\n", ships_destroyed);

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
int validate() {
	if(already_used()) { //check if input has already been used
		printw("You already used that coord!\n");
		memset(&input, 0, sizeof(input));
		return 0;
	}
	/*printf("\nValidating coord...\n");
	printf("coord[0] = %c\n", coord[0]);
	printf("coord[1] = %c\n", coord[1]);*/

	if((input[0] == 'A' || input[0] == 'B' || input[0] == 'C' || input[0] == 'D') &&
		(input[1] == '1' || input[1] == '2' || input[1] == '3' || input[1] == '4')) { //coord valid
		//CHECK TO MAKE SURE coord[3] IS "\n"!
		//printf("coord is valid!\n");
		return 1;

	} else {
		printw("That coordinate is invalid!\n");
		refresh();
		memset(&input, 0, sizeof(input)); //clear coord buf
		return 0;
	}
	return 0;
}

/* Randomly add ships to empty board on start of game */
int init_board(int seed) {
	int ships = 0;
	int ships_to_place = MAX_SHIPS;

	while(ships < MAX_SHIPS) {
		clear();
		char row_letter = 'A';
		printw("Arrange your ships...\n");
		printw("\tShips to place: %d\n\n", ships_to_place);
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
		int valid = 0;
		while(valid == 0) { //ask for coordinate until user input is formatted correctly
			printw("Place a ship at coordinate: ");
			refresh();
			getstr(input);
			valid = validate();
		}
		if(already_used()) {
			printw("\nYou already placed a ship at %s.\n", input);
			refresh();
			sleep(WAIT);
			clear();
			continue;
		}
		strcpy(old_inputs[ships], input);

		int row;
		int col;

		//parse coord string for numeric coordinates
		switch(input[0]) {
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
		sscanf(&input[1], "%d", &row);
		row -= 1;

		board[col][row] = SHIP;
		ships += 1;
		ships_to_place -= 1;
	}

	memset(&old_inputs, 0, sizeof(old_inputs)); //clear old input array
	return ships;
}

/* Sends coord to other player */
void send_coord(int fd) {
	memset(&out_coord, 0, sizeof(out_coord));
	int valid = 0;
	while(valid == 0) { //ask for coordinate until user input is formatted correctly
		printw("Fire at coordinate: ");
		refresh();
		getstr(input);
		valid = validate();
	}
	strcpy(old_inputs[old_inputs_index], input); //store accepted input in previously used input array
	old_inputs_index += 1;
	strcpy(out_coord, input);
	return;
}

void *read_data(void *arg) {
	int *fd = (int *)arg;
	while(1) {
		read(*fd, &in_coord, sizeof(in_coord));
	}
}

void *write_data(void *arg) {
	int *fd = (int *)arg;
	while(1) {
		write(*fd, &out_coord, sizeof(out_coord));
	}
}

/* Reads and processes sent coord */
void read_coord(int fd) {
	memset(&in_coord, 0, sizeof(in_coord));
	while(1) {
		if(in_coord[0] == 0) {
			sleep(1);
		} 
		else if(strcmp(in_coord, last) == 0) { //program won't reread old input and throw off game
			sleep(1);
		}
		else if(strcmp(&in_coord[0], FAIL) == 0) {
			success(fd);
		}
		else if(strcmp(&in_coord[0], HIT) == 0) {
			printw("Your strike was successful!");
			refresh();
			sleep(WAIT);
			ships_destroyed += 1;
			print_display();
		}
		else if(strcmp(&in_coord[0], MISS) == 0) {
			printw("Your strike missed...");
			refresh();
			sleep(4);
			print_display();
		}
		else {
			strcpy(last, in_coord);
			check_board(fd);
			print_display();
			return;
		}
	}
}

/* Checks board at recieved coord and updates board accordingly */
void check_board(int fd) {
	int row;
	int col;

	//parse coord string for numeric coordinates
	switch(in_coord[0]) {
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

	sscanf(&in_coord[1], "%d", &row);
	row -= 1;

	clear();
	printw("\n\nIncoming missile strike at %s!\n\n", in_coord);
	refresh();
	sleep(WAIT);
	//check board at coord
	if(board[col][row] != EMPTY) { //ship is hit
		clear();
		printw("\n\nYour ship has sunk!\n\n");
		board[col][row] = 0;
		ships_remaining -= 1;
		strcpy(out_coord, HIT);
	} else { //ship is not hit
		clear();
		printw("\n\nThe strike missed!\n\n");
		strcpy(out_coord, MISS);
	}
	refresh();
	sleep(WAIT);

	if(ships_remaining == 0) { //player has reached fail state
		failure(fd);
	}
	return;
}

int already_used() {
	for(int i = 0; i < STORED_INPUTS; i++) {
		if(strcmp(input, old_inputs[i]) == 0) {
			return 1;
		}
	}
	return 0;
}

void start_screen() {
	clear();
	printw("\n\nGame is starting...");
	refresh();
	sleep(WAIT);
}

/* Player fails - disconnects from server, prints fail state */
void failure(int fd) {
	strcpy(out_coord, "F");

	clear();
	printw("\n\nAll of your ships have been sunk...\n\n");
	refresh();
	sleep(WAIT);
	printw("YOU LOSE.");
	refresh();
	sleep(WAIT);
	strcpy(out_coord, FAIL);

	close(fd);
	endwin();
	exit(0);
}

/* Player wins - disconnects from server, prints success state */
void success(int fd) {
	clear();
	printw("\n\nYou sunk all of Player %d's ships...\n\n", other_player);
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
