#include "common.h"
#include "battleship.h"

#define MAXLINE 8192
#define LISTENQ 1024

int main(int argc, char** argv) {
	char *port;
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];

	//pthread_t thread_id;

	if(argc != 2) {
		error_exit("No listening port provided");
	}

	port = argv[1]; //set port

	if((listenfd = open_server(port)) < 0) { //get listen file descriptor
		error_exit("Could not open server on provided port");
	}

	/* Begin accepting connections */
	printf("\n\nHosting BATTLE SHIP on port %s\n", port);
	printf("Waiting for another player to connect...\n\n");
	clientlen = sizeof(struct sockaddr_storage);
	while(1) {
		connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
		getnameinfo((struct sockaddr *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
		printf("New player connected from (%s, %s)!\n", client_hostname, client_port);
		sleep(WAIT);
		//pthread_create(&thread_id, NULL, begin_game_server, &connfd);
		begin_game(connfd, PLAYER_ONE);
	}

	return 0;
}