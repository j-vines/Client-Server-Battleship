#include "common.h"
#include "menu.h"
#include "battleship.h"

#define HOST 0
#define JOIN 1
#define INSTRUCTIONS 2
#define QUIT 3

/* bs.c contains driver and functions for main menu */

char name[25];
char choice;
char in[100];

/* Catch and ignore SIGINT -- player can't use ctrl-c to close program */
void sig_handler(int sig) {
	return;
}

/* Driver for Battleship - displays main menu */
/* From main menu you can choose to HOST or JOIN an existing game,
   view the instructions for the game, or exit the program. Navigate menu
   with numeric keys 1-4 */
int main() {
	int option = HOST; //default option is host on start

	signal(SIGINT, sig_handler); //ignore SIGINT
	init_curse();
	curs_set(0);
	
	while(1) {
		noecho();
		memset(&in, 0, sizeof(in));
		display_menu(option);
		int nav = getch();
		
		switch(nav) {
			case KEY_UP:
				if(option == HOST) { //already at top of list
					break;
				} else {
					option -= 1; //navigate to next option up
					break;
				}
			case KEY_DOWN:
				if(option == QUIT) { //already at bottom of list
					break;
				} else {
					option += 1; //navigate to next option down
					break;
				}
			case 10: //enter key
				if(option == HOST) host();
				else if(option == JOIN) join();
				else if(option == INSTRUCTIONS) instructions();
				else quit();
				break;
			default:
				break;
		}
	}
}

/* Displays main menu with options to host game, join game, or view leaderboards */
void display_menu(int choice) {
	clear();
	printw("\n\n   __________________________________   ________________________________\n");
	printw("   |  _  ||     ||      ||      ||  |   |     ||     ||  |  ||  ||     |\n");
	printw("   |   __!|  _  |!__  __!!__  __!|  |   |  ___!|  ___!|  !  ||  ||  -  |\n");
	printw("   |  _  ||     |  |  |    |  |  |  !___|  __!_!__   ||     ||  ||  ___!\n");
	printw("   |     ||  |  |  |  |    |  |  |     ||     ||     ||  |  ||  ||  |   \n");
	printw("   !_____!!__!__!  !__!    !__!  !_____!!_____!!_____!!__!__!!__!!__!   \n\n");                                                                 
	printw("		____________________________________________\n");
	printw("		|                                          |\n");
	printw("		|   Would you like to...                   |\n");
	printw("		|                                          |\n");
	if(choice == HOST) {
		printw("		|          ");
		attron(COLOR_PAIR(GREEN));
		printw("HOST GAME");
		attroff(COLOR_PAIR(GREEN));
		printw("                       |\n");
		printw("		|          JOIN GAME                       |\n");
		printw("		|          SEE INSTRUCTIONS                |\n");
		printw("		|                                          |\n");
		printw("		|          EXIT                            |\n");
		printw("		!__________________________________________!\n");
	}
	else if(choice == JOIN) {
		printw("		|          HOST GAME                       |\n");
		printw("		|          ");
		attron(COLOR_PAIR(GREEN));
		printw("JOIN GAME");
		attroff(COLOR_PAIR(GREEN));
		printw("                       |\n");
		printw("		|          SEE INSTRUCTIONS                |\n");
		printw("		|                                          |\n");
		printw("		|          EXIT                            |\n");
		printw("		!__________________________________________!\n");
	}
	else if(choice == INSTRUCTIONS) {
		printw("		|          HOST GAME                       |\n");
		printw("		|          JOIN GAME                       |\n");
		printw("		|          ");
		attron(COLOR_PAIR(GREEN));
		printw("SEE INSTRUCTIONS");
		attroff(COLOR_PAIR(GREEN));
		printw("                |\n");
		printw("		|                                          |\n");
		printw("		|          EXIT                            |\n");
		printw("		!__________________________________________!\n");
	}
	else {
		printw("		|          HOST GAME                       |\n");
		printw("		|          JOIN GAME                       |\n");
		printw("		|          SEE INSTRUCTIONS                |\n");
		printw("		|                                          |\n");
		printw("		|          ");
		attron(COLOR_PAIR(GREEN));
		printw("EXIT");
		attroff(COLOR_PAIR(GREEN));
		printw("                            |\n");
		printw("		!__________________________________________!\n");
	}
	refresh();

}

/* Asks for port number to host on, then creates server/begins accepting connections */
void host() {
	char port[5];
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];

	// Ask for port
	clear();
	echo();
	printw("\n\n");
	printw("	 _____  _____  _____  _____    _____  _____  _____  _____ \n");
	printw("	|  |  ||     ||   __||_   _|  |   __||  _  ||     ||   __|\n");
	printw("	|     ||  |  ||__   |  | |    |  |  ||     || | | ||   __|\n");
	printw("	|__|__||_____||_____|  |_|    |_____||__|__||_|_|_||_____|\n\n");                                                   
	printw("	Port to host on: ");
	getstr(port);
	//strcpy(port, in);

	if((listenfd = open_server(port)) < 0) { //get listen file descriptor
		printw("	Could not open server on provided port");
		refresh();
		sleep(2);
		return;
	}
	
	/* Begin accepting connections */
	clear();
	printw("\n\nHosting BATTLESHIP on port %s\n", port);
	printw("Waiting for another player to connect...\n\n");
	refresh();
	clientlen = sizeof(struct sockaddr_storage);
	while(1) {
		connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
		getnameinfo((struct sockaddr *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
		printw("New player connected from (%s, %s)!\n", client_hostname, client_port);
		refresh();
		sleep(WAIT);

		//connection has been made -- begin game
		begin_game(&connfd, PLAYER_ONE);
		return;
	}
}

/* Asks for ip address/port number to connect to, then creates client to connect to server */
void join() {
	int port, output_fd;
	char host[100];
	char port_str[100];
	clear();
	echo();
	printw("\n\n");
	printw("	    __  _____  _____  _____    _____  _____  _____  _____ \n");
	printw("	 __|  ||     ||     ||   | |  |   __||  _  ||     ||   __|\n");
	printw("	|  |  ||  |  ||-   -|| | | |  |  |  ||     || | | ||   __|\n");
	printw("	|_____||_____||_____||_|___|  |_____||__|__||_|_|_||_____|\n\n");                                                              
	printw("	Host to connect to: ");
	refresh();
	getstr(host);
	clear();
	printw("\n\n");
	printw("	    __  _____  _____  _____    _____  _____  _____  _____ \n");
	printw("	 __|  ||     ||     ||   | |  |   __||  _  ||     ||   __|\n");
	printw("	|  |  ||  |  ||-   -|| | | |  |  |  ||     || | | ||   __|\n");
	printw("	|_____||_____||_____||_|___|  |_____||__|__||_|_|_||_____|\n\n");
	printw("	Host: %s\n", host);
	printw("	Port to connect to: ");
	refresh();
	getstr(port_str);
	sscanf(port_str, "%d", &port); //set port

	if((output_fd = connect_server(host, port)) < 0) { //get file descriptor for writing to server
		printw("	Could not connect to server at provided hostname and port");
		refresh();
		sleep(2);
		return;
	}
	clear();
	printw("\n\nYou joined %s's BATTLESHIP game!\n", host);
	refresh();
	sleep(WAIT);

	//connected to server -- begin game
	begin_game(&output_fd, PLAYER_TWO);
	return;
}

/* Displays instructions */
void instructions() {
	clear();
	printw("\n\n");
	printw("	 _____  _____  _ _ _    _____  _____    _____  __     _____  __ __ \n");
	printw("	|  |  ||     || | | |  |_   _||     |  |  _  ||  |   |  _  ||  |  |\n");
	printw("	|     ||  |  || | | |    | |  |  |  |  |   __||  |__ |     ||_   _|\n");
	printw("	|__|__||_____||_____|    |_|  |_____|  |__|   |_____||__|__|  |_|  \n\n");
	printw("	Press any key to return... \n\n");
	printw("	Creating a game:\n");
	printw("	To host a game, pick HOST GAME at the main menu, then type a listening\n");
	printw("	port to host a game on. A player who wishes to join your game must \n");
	printw("	have your IP address as well as the number of the port you chose to \n");
	printw("	host the server on. Once a connection is made, your game will begin.\n\n");
	printw("	Playing the game:\n");
	printw("	BATTLESHIP is a turn based 2 player game where each player has a coord-\n");
	printw("	inate grid and 5 ships. Once your ships are arranged on your grid, you\n");
	printw("	and your opponent will take turns firing missiles at coordinates on the\n");
	printw("	other person's grid. The player who destroys all of the other's ships\n");
	printw("	with missiles first wins.\n\n");
	printw("	Coordinates should be formatted as \"A1\", \"C4\", etc.\n");                                                             
    refresh();
    getch();                                                                                
	return;
}

/* Quits game */
void quit() {
	endwin();
	exit(0);
}
