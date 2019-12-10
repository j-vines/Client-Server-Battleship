#ifndef battleship_h
#define battleship_h

/* battleship.h contains constants, global variables, and prototypes for functions used during gameplay */

/* Declaration of constants */
#define PLAYER_ONE 1
#define PLAYER_TWO 2
#define WAIT 4
#define BUFF_SIZE 4
#define MAX_SHIPS 5
#define DESTROYED 2 
#define SHIP 1		
#define EMPTY 0
#define BOARD_LENGTH 4
#define BOARD_WIDTH 4
#define HIT "H"
#define MISS "M"
#define FAIL "F"
#define READY "R"
#define I_QUIT "Q"
#define STORED_INPUTS 20
#define WIN 0
#define FORFEIT 1
#define WAITING 0
#define SENDING 1

/* color pair IDs */
#define GREEN 1	
#define RED 2	
#define YELLOW 3

/* Both player and server get randomly initialized board */
/* Values can either be SHIP if space contains ship, or EMPTY if space is empty */
int board[BOARD_LENGTH][BOARD_WIDTH];
int ships_remaining; 
int ships_destroyed;
int other_player;
int gameover;
int state;

/* Input and output buffers for reading/writing coordinates */
char last[BUFF_SIZE];
char in_coord[BUFF_SIZE];
char out_coord[BUFF_SIZE];
char input[BUFF_SIZE];
int old_inputs_index;
char old_inputs[STORED_INPUTS][BUFF_SIZE]; //array to hold inputs already tried

/* Client begins game after connecting to server */
void begin_game(int *fd, int player);

/* Creates and binds file descriptor to provided port to begins listening for clients */
int open_server(char *port);

/* Client attempts to connect to server as provided host and port */
int connect_server(char *host, int port);

/* Print player's board to curses window */
void print_display(); 

/* Check board for ship at given coord, return 1 if coord is valid format, return 0 otherwise */
int validate();

/* Add ships to empty board on start of game */
int init_board(int seed);

/* Sends coord to other player */
void send_coord(int fd);

/* Reads and processes sent coord */
void process_coord(int fd);

/* Constantly reads contents from socket into in_coord concurrently */
void *read_data(void *fd);

/* Constantly writes contents of out_coord to socket concurrently */
void *write_data(void *fd);

/* Checks board at recieved coord and updates board accordingly */
void check_board(int fd);

/* Checks if input has already been used by searching array of stored inputs */
int already_used();

/* Displays the game start screen */
void start_screen(int player);

/* Player fails - disconnects from server, prints fail state */
void failure(int fd);

/* Player wins - disconnects from server, prints success state */
void success(int fd, int condition);

/* Exit and error, print error message to standard output */
void error_exit(char *msg);

/* Wait for other player to send READY */
void wait_for_ready(int *fd);

/* Prints current state of game board */
void print_board();

/* Initialize ncurses screen */
void init_curse();

/* Resets game board and all changed instance variables */
void reset_game();

/* Sets gameover to 1 and resets game state so that program returns to main menu and game can be played again */
void return_to_menu();

#endif
