#ifndef battleship_h
#define battleship_h

/* Declaration of constants */
#define MAX_SHIPS 5
#define MIN_SHIPS 3

#define SHIP 1
#define EMPTY 0

#define BOARD_LENGTH 4
#define BOARD_WIDTH 4

/* Both player and server get randomly initialized board */
/* Values can either be SHIP if space contains ship, or EMPTY if space is empty */
int board[BOARD_LENGTH][BOARD_WIDTH];

/* Input of coord where player thinks server's ship is */
/* coord[0] = A, B, C, or D
   coord[1] = 1, 2, 3, or 4 */
char coord[4];

/* Client begins game after connecting to server */
void begin_game_client(int fd);

/* Server creates thread and begins game once client is connected */
void *begin_game_server(void *fd);

/* Creates and binds file descriptor to provided port to begins listening for clients */
int open_server(char *port);

/* Client attempts to connect to server as provided host and port */
int connect_server(char *host, int port);

/* Print player's board to standard output */
void print_display(int round, int ships_remaining, int ships_destroyed); 

/* New turn - ask player for coordinate */
void turn();

/* Check board for ship at given coord, return 1 if coord is valid format, return 0 otherwise */
int validate(char *coord);

/* Randomly add ships to empty board on start of game */
int init_board();

/* Server generates random coord to send to client */
char *gen_coord();

/* Sends coord to server */
void send_coord(char *coord);

/* Reads coord from server */
char* read_coord();

/* Player fails - disconnects from server, prints fail state */
void failure();

/* Player wins - disconnects from server, prints success state */
void success();

/* Exit and error, print error message to standard output */
void error_exit(char *msg);

#endif