/* NETWORKING */
#define MTU 500
#define PORT 10000
#define IP_ADDRESS "127.0.0.1"

#define PING_MESSAGE 0
#define CONNECT_MESSAGE 1
#define START_GAME_MESSAGE 2
#define TURN_MESSAGE 3
#define PLAY_MESSAGE 5
#define STATE_MESSAGE 6
#define END_GAME_MESSAGE 7
#define CONFIRMATION_MESSAGE 8

#define WIN 'W'
#define LOSE 'L'
#define TIE 'T'


/* GAME CONSTANTS */
#define USERNAME_LENGTH 10

#define NUMBER_OF_COLUMNS 7
#define NUMBER_OF_ROWS 6
#define NUMBER_TO_WIN 4

#define EMPTY_MARKER '\0'
#define MARKER1 '\1'
#define MARKER2 '\2'

#define MOVE_INDICATOR 1 /* Indicates where its players turn */
#define WAIT_INDICATOR 0 /* Indicates where its not players turn */

#define TIME_TO_PING 10
#define TIME_TO_RECONNECT 30

/* OTHER */
#define MIN(a,b) ((a) < (b)) ? (a) : (b)