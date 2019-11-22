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

	pthread_t thread_id;

	if(argc != 2) {
		error_exit("No listening port provided");
	}

	port = argv[1]; //set port

	if((listenfd = open_server(port)) < 0) { //get listen file descriptor
		error_exit("Could not open server on provided port");
	}

	/* Begin accepting connections */
	printf("Waiting for players to connect...\n");
	clientlen = sizeof(struct sockaddr_storage);
	while((connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen))) {
		getnameinfo((struct sockaddr *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
		printf("Player connected from (%s, %s)\n", client_hostname, client_port);
		pthread_create(&thread_id, NULL, begin_game_server, &connfd);
	}

	return 0;
}