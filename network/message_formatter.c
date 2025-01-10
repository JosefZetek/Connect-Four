#include "message_formatter.h"

int confirmation_message(char * buffer, int * length, int message_type) {

    if(!buffer || !length)
        return 0;

    buffer[0] = CONFIRMATION_MESSAGE;
    buffer[1] = message_type;
    *length = 2;
    return 1;
}

int registered_message(char * buffer, int * length) {
    if(!buffer || !length)
        return 0;

    buffer[0] = REGISTERED_MESSAGE;
    *length = 1;

    return 1;
}

int start_game_message(char * buffer, int * length, int player) {
    if(!buffer || !length)
        return 0;

    buffer[0] = START_GAME_MESSAGE;
    buffer[1] = player%2 ? MARKER1 : MARKER2;
    buffer[2] = player ? MOVE_INDICATOR : WAIT_INDICATOR;
    *length = 3;

    return 1;
}

int instruction_message(char * buffer, int * length, int player) {
    if(!buffer || !length)
        return 0;

    buffer[0] = TURN_MESSAGE;
    buffer[1] = player;
    *length = 2;

    return 1;
}