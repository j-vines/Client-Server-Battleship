#include "battleship.h"
#include "common.h"

/* Client begins game after connecting to server */
void begin_game_client(int fd) {
	sleep(2); //sleep so init_board does not get seeded with same time as server
	init_board();
	print_display();
}

/* Server creates thread and begins game once client is connected */
void *begin_game_server(void *fd) {
	int *to_client = (int*)fd;
	printf("Game begun on fd: %d\n", *to_client);
	
	init_board();
	//print_display();
	return NULL;
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
	return;
}

/* New turn - ask player for coordinate */
void turn() {
	return;
}

/* Check board for ship at given coord, return 1 if coord contains ship, return 0 otherwise */
int validate(char *coord) {
	return 0;
}

/* Randomly add ships to empty board on start of game */
void init_board() {
	srand(time(NULL)); //seed random num gen
	int ships = 0;

	for(int row = 0; row < BOARD_LENGTH; row++) {
		for(int col = 0; col < BOARD_WIDTH; col++) {
			int num = rand() % 10;
			if(num <= 3) { 
				board[row][col] = SHIP; //place ship on board
				ships += 1;
			}
			else {
				board[row][col] = EMPTY;
			}
		}
	}

	printf("Placed %d ships on board...\n", ships);
	return;
}

/* Server generates random coord to send to client */
char *gen_coord() {
	return NULL;
}

/* Sends coord to server */
void send_coord(char *coord) {
	return;
}

/* Reads coord from server */
char* read_coord() {
	return NULL;
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
