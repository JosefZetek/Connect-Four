#include "message_formatter.h"

int start_game_message(char * buffer, int * length, int player) {
    if(!buffer || !length)
        return 0;

    buffer[0] = START_GAME_MESSAGE;
    buffer[1] = player%2 ? MARKER1 : MARKER2;
    *length = 2;

    return 1;
}

int turn_message(char * buffer, int * length) {
    if(!buffer || !length)
        return 0;

    buffer[0] = TURN_MESSAGE;

    *length = 1;
    return 1;
}

int state_message(char * buffer, int * length, struct game_structure * game, int column) {
    int i;

    if(!buffer || !length || !game || column < 0 || column >= NUMBER_OF_COLUMNS)
        return 0;

    buffer[0] = STATE_MESSAGE;

    /* Insert rows in a given column */
    for(i = 0; i<NUMBER_OF_ROWS; i++) {
        printf("[%d][%d] = %d\n", i, column, game->board[i][column]);
        buffer[i + 1] = (char)game->board[i][column];
    }

    

    *length = NUMBER_OF_ROWS + 1;

    return 1;
}

int end_game_message(char * buffer, int * length, char result) {
    if(!buffer || !length)
        return 0;

    buffer[0] = END_GAME_MESSAGE;
    buffer[1] = result;

    *length = 2;

    return 1;
}




