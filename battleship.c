#include "battleship.h"
#include "common.h"

/* Server creates thread and begins game once client is connected */
void *begin_game(void *fd) {
	int *to_client = (int*)fd;
	printf("Game begun on fd: %d\n", *to_client);
	return NULL;
}

/* Client attempts to connect to server as provided host and port */
void connect_server() {
	return;
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
