#include "battleship.h"
#include "common.h"

//NEED: communication between processes/threads and file I/O and signals
//Create file to store number of wins per person... usernames
//If someone uses ctrl-C, quit game for both players and whoever forfeited loses

/* battleship.c contains functions used during game play */

/* Client begins game after connecting to server */
void begin_game(int *fd, int player) {
	pthread_t read_id;
	pthread_t write_id;
	gameover = 0; //gameover is 0 at start of game, set to 1 once fail state is reached

	if(player == PLAYER_ONE) {
		other_player = PLAYER_TWO;
	} else {
		other_player = PLAYER_ONE;
	}

	start_screen(player); //show title screen

	//player places their boards
	ships_remaining = init_board(player); 
	ships_destroyed = 0;

	//Make sure both players are ready
	wait_for_ready(fd);


	//display game startint screen
	clear();
	printw("\n\nGame is starting...");
	refresh();
	sleep(WAIT);

	//create reading and writing processes
	pthread_create(&read_id, 0, read_data, (void *)fd);
	pthread_create(&write_id, 0, write_data, (void *)fd);

	//main game loop
	while(ships_remaining > 0) {
		
		print_display();
		
		if(player == PLAYER_ONE) { //Player 1's loop
			send_coord(*fd);
			read_coord(*fd);
			if(gameover == 1) return; //return to main menu
		} else { //Player 2's loop
			read_coord(*fd);
			if(gameover == 1) return; //return to main menu
			send_coord(*fd);
		}
	}
	return;
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

/* Print player's board to curses window */
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

	if((input[0] == 'A' || input[0] == 'B' || input[0] == 'C' || input[0] == 'D') &&
		(input[1] == '1' || input[1] == '2' || input[1] == '3' || input[1] == '4')) { //coord valid
		return 1;

	} else {
		printw("That coordinate is invalid!\n");
		refresh();
		memset(&input, 0, sizeof(input)); //clear coord buf
		return 0;
	}
	return 0;
}

/* Add ships to empty board on start of game */
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

/* Constantly reads contents from socket into in_coord concurrently */
void *read_data(void *arg) {
	int *fd = (int *)arg;
	while(1) {
		if(gameover == 1) { //close reading thread on game over
			pthread_exit(0);
		}
		read(*fd, &in_coord, sizeof(in_coord));
	}
}

/* Constantly writes contents of out_coord to socket concurrently */
void *write_data(void *arg) {
	int *fd = (int *)arg;
	while(1) {
		if(gameover == 1) { //close writing thread on game over
			pthread_exit(0);
		}
		write(*fd, &out_coord, sizeof(out_coord));
	}
}

/* Processes coord recieved into in_coord */
void read_coord(int fd) {
	memset(&in_coord, 0, sizeof(in_coord));
	int recieved = 0;
	while(1) {
		if(in_coord[0] == 0) {
			sleep(1);
		} 
		else if(strcmp(in_coord, last) == 0) { //program won't reread old input and throw off game
			sleep(1);
		}
		else if(strcmp(&in_coord[0], FAIL) == 0) {
			success(fd);
			return;
		}
		
		// recieves notification of attack hit or miss
		// first time notification is recieved, notify player
		// if notification is recieved again, ignore it
		else if(strcmp(&in_coord[0], HIT) == 0) {
			if(recieved == 0) {
				recieved = 1;
				printw("Your strike was successful!");
				refresh();
				sleep(WAIT);
				ships_destroyed += 1;
				print_display();
			} else { // already notified, ignore
				sleep(1);
			}
			
		}
		else if(strcmp(&in_coord[0], MISS) == 0) {
			if(recieved == 0) {
				recieved = 1;
				printw("Your strike missed...");
				refresh();
				sleep(4);
				print_display();
			} else { // already notified, ignore
				sleep(1);
			}
			
		}

		// new coord has been recieved -- check board and print display
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

/* Checks if input coord has already been used by iterating through array of previously used coords */
int already_used() {
	for(int i = 0; i < STORED_INPUTS; i++) {
		if(strcmp(input, old_inputs[i]) == 0) {
			return 1;
		}
	}
	return 0;
}

/* Text displayed before start of game */
void start_screen(int player) {
	clear();
	printw("\n\n");                                                                
	printw("	 _____  _____  _____  _____  __     _____  _____  _____  _____  _____ \n");
	printw("	| __  ||  _  ||_   _||_   _||  |   |   __||   __||  |  ||     ||  _  |\n");
	printw("	| __ -||     |  | |    | |  |  |__ |   __||__   ||     ||-   -||   __|\n");
	printw("	|_____||__|__|  |_|    |_|  |_____||_____||_____||__|__||_____||__|   \n");
	printw("	IS BEGINNING...");                                                            
	refresh();
	sleep(WAIT);
	clear();
	printw("	You are Player %d\n\n", player);
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
	printw("	YOU LOSE.");
	refresh();
	sleep(WAIT);
	strcpy(out_coord, FAIL);
	gameover = 1;
	return;
}

/* Player wins - disconnects from server, prints success state */
void success(int fd) {
	clear();
	printw("\n\nYou sunk all of Player %d's ships...\n\n", other_player);
	refresh();
	sleep(WAIT);
	printw("	YOU WIN!");
	refresh();
	sleep(WAIT);
	gameover = 1;
	return;
}

/* Exit and error, print error message to standard output */
void error_exit(char *msg) {
	printf("Error: %s\n" , msg);
	exit(1);
}

/* Wait for other player to send READY */
void wait_for_ready(int *fd) {
	clear();
	printw("Hit any key when you're ready to play!");
	getch();
	strcpy(out_coord, READY);
	write(*fd, &out_coord, sizeof(out_coord));
	memset(&out_coord, 0, sizeof(in_coord));

	clear();
	printw("Waiting for Player %d...", other_player);
	refresh();

	while(1) {
		if(strcmp(&in_coord[0], READY) == 0) { //Once READY, end reading process
			memset(&in_coord, 0, sizeof(in_coord));
			return;
		}
		read(*fd, &in_coord, sizeof(in_coord));
	}
}

/* Initialize ncurses screen */
void init_curse() {
	initscr();
	cbreak();
	clear();
	refresh();
}
