#pragma once

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vector.h"
#include "unconfirmed_request.h"

#include "../constants.h"



struct game_structure {
    struct sockaddr_in * player1_addr;
    struct sockaddr_in * player2_addr;

    char username_player1[USERNAME_LENGTH+1];
    char username_player2[USERNAME_LENGTH+1];

    int current_player;
    char board [NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];

    time_t last_ping_player1;
    time_t last_ping_player2;

    struct vector * unconfirmed_requests;
};

/**
 * @brief Initialize a new game structure
 * @return Returns a pointer to the new game structure
 */
struct game_structure * game_init();

/**
 * @brief Function assigns a player to a game
 * @param game The game structure to assign the player to
 * @param client_addr The address of the client that sent the message
 * @param buffer The message that was sent
 * @param message_length The length of the message
 * @return Returns 1 if the player was assigned successfully, 0 otherwise
 */
int assign_player(struct game_structure * game, struct sockaddr_in * client_addr, char * buffer, int message_length);

/**
 * @brief Frees memory allocated for the game structure and marks the pointer as NULL
 * @param game Pointer to pointer to the game structure to deallocate
 */
void game_deallocate(struct game_structure ** game);

/**
 * @brief Current players move
 * @param game The game structure
 * @param column The column to play
 */
int play_column(struct game_structure * game, int column);


/**
 * @brief Function checks if the game is tie
 * @param game The game structure
 * @return Returns 1 if it is a tie, 0 otherwise
 */
int check_tie(struct game_structure * game);


/**
 * @brief Function checks if the current player won
 * @param game The game structure
 * @param column Column he chosed in his most recent turn
 * @return Returns 1 if he won the game
 */
int check_win(struct game_structure * game, int column);
