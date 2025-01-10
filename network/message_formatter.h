#include "../data_structures/game_structure.h"
#include "../constants.h"

int start_game_message(char * buffer, int * length, int player);

int turn_message(char * buffer, int * lenght);

int state_message(char * buffer, int * length, struct game_structure * game, int column);

int end_game_message(char * buffer, int * length, char result);
