#include "common.h"
#include "battleship.h"

int main(int argc, char** argv) {
	//TODO: Ask player for name, store
	// argv contains hostname and port for server
	// connect to server
	int port, output_fd;
	char *host;
	if(argc != 3) {
		error_exit("Must provide server host and port");
	}

	host = argv[1]; //set host
	sscanf(argv[2], "%d", &port); //set port

	if((output_fd = connect_server(host, port)) < 0) { //get file descriptor for writing to server
		error_exit("Could not connect to server at provided hostname and port");
	}

	init_curse();

	printw("\n\nYou joined %s's BATTLE SHIP game!\n", host);
	refresh();
	sleep(WAIT);
	begin_game(output_fd, PLAYER_TWO);

	return 0;
}