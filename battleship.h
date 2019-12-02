#ifndef battleship_h
#define battleship_h

/* Declaration of constants */
#define PLAYER_ONE 1
#define PLAYER_TWO 2
#define WAIT 4
#define MAX_SHIPS 2
#define SHIP 1
#define EMPTY 0
#define BOARD_LENGTH 4
#define BOARD_WIDTH 4
#define HIT "H"
#define MISS "M"
#define FAIL "F"
#define STORED_INPUTS 20

/* Both player and server get randomly initialized board */
/* Values can either be SHIP if space contains ship, or EMPTY if space is empty */
int board[BOARD_LENGTH][BOARD_WIDTH];

int ships_remaining;
int ships_destroyed;
int other_player;

/* Input and output buffers for reading/writing coordinates */
char last[4];
char in_coord[4];
char out_coord[4];
char input[4];

int old_inputs_index;
char old_inputs[STORED_INPUTS][4]; //array to hold inputs already tried

/* Client begins game after connecting to server */
void begin_game(int *fd, int player);

/* Creates and binds file descriptor to provided port to begins listening for clients */
int open_server(char *port);

/* Client attempts to connect to server as provided host and port */
int connect_server(char *host, int port);

/* Print player's board to standard output */
void print_display(); 

/* Check board for ship at given coord, return 1 if coord is valid format, return 0 otherwise */
int validate();

/* Randomly add ships to empty board on start of game */
int init_board(int seed);

/* Sends coord to other player */
void send_coord(int fd);

/* Reads and processes sent coord */
void read_coord(int fd);

void *read_data(void *fd);

void *write_data(void *fd);

/* Checks board at recieved coord and updates board accordingly */
void check_board(int fd);

/* Checks if input has already been used by searching array of stored inputs */
int already_used();

/* Displays the game start screen */
void start_screen();

/* Player fails - disconnects from server, prints fail state */
void failure(int fd);

/* Player wins - disconnects from server, prints success state */
void success(int fd);

/* Exit and error, print error message to standard output */
void error_exit(char *msg);

/* Initialize ncurses screen */
void init_curse();

#endif