#include "common.h"
#include "battleship.h"

#define MAXLINE 8192
#define LISTENQ 1024

int main(int argc, char** argv) {
	char *port;
	int listenfd, connfd;
	struct addrinfo hints, *listp, *p;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];

	pthread_t thread_id;

	if(argc != 2) {
		error_exit("No listening port provided");
	}

	port = argv[1]; //set port

	/* Create listenfd */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;

	if(getaddrinfo(NULL, port, &hints, &listp) != 0) {
		error_exit("addrinfo");
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
		error_exit("listen");
	}

	/* Begin accepting connections */
	printf("Waiting for players to connect...\n");
	clientlen = sizeof(struct sockaddr_storage);
	while((connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen))) {
		getnameinfo((struct sockaddr *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
		printf("Player connected from (%s, %s)\n", client_hostname, client_port);
		pthread_create(&thread_id, NULL, begin_game, &connfd);
	}

	return 0;
}