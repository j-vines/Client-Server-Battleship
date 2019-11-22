#include "battleship.h"
#include "common.h"

/* Server creates thread and begins game once client is connected */
void *begin_game(void *fd) {
	int *to_client = (int*)fd;
	printf("Game begun on fd: %d\n", *to_client);
	return NULL;
}

/* Creates and binds file descriptor to provided port to begins listening for clients */
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
	return;
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
