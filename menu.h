#ifndef menu_h
#define menu_h

/* Displays main menu with options to host game, join game, or view leaderboards */
void display_menu();

/* Asks for port number to host on, then creates server/begins accepting connections */
void host();

/* Asks for ip address/port number to connect to, then creates client to connect to server */
void join();

/* Displays instructions */
void instructions();

/* Quits game */
void quit();

#endif