#include "game_structure.h"


/* --------------------STATIC FUNCTIONS--------------------*/

static void reset_game(struct game_structure * game) {
    int i,j;

    for(i = 0; i<NUMBER_OF_ROWS; i++) {
        for(j = 0; j<NUMBER_OF_COLUMNS; j++)
            game->board[i][j] = EMPTY_MARKER;
    }
}

static int __get_empty_row(struct game_structure * game, int column_index) {
    int i;

    for(i = (NUMBER_OF_ROWS-1); i >= 0; i--) {
        if(game->board[i][column_index] == EMPTY_MARKER)
            return i;
    }

    return -1;
}

static int check_column(struct game_structure * game, int row_index, int column_index, char player_marker) {
    int i, player_streak = 0;
    int increment = MIN(NUMBER_TO_WIN -1, (NUMBER_OF_ROWS - 1) - row_index);

    for(int i = 0; i<=increment; i++) {
        if(game->board[row_index + i][column_index] != player_marker) {
            player_streak = 0;
            continue;
        }
            

        if(++player_streak == NUMBER_TO_WIN)
            return 1;
    }
    return 0;
}

static int check_row(struct game_structure * game, int row_index, int column_index, char player_marker) {
    int i, player_streak = 0;

    int decrement = MIN(column_index, NUMBER_TO_WIN - 1);
    int increment = MIN((NUMBER_OF_COLUMNS - 1) - column_index, NUMBER_TO_WIN - 1);

    for(int i = -decrement; i<=increment; i++) {
        if(game->board[row_index][column_index + i] != player_marker) {
            player_streak = 0;
            continue;
        }

        if(++player_streak == NUMBER_TO_WIN)
            return 1;
    }
    return 0;
}

static int check_diagonals(struct game_structure * game, int row_index, int column_index, char player_marker) {
    int i, player_streak = 0;

    int decrement = MIN(MIN(column_index, row_index), NUMBER_TO_WIN - 1);
    int increment = MIN(MIN((NUMBER_OF_COLUMNS - 1) - column_index, (NUMBER_OF_ROWS - 1) - row_index), NUMBER_TO_WIN - 1);

    for(i = -decrement; i<=increment; i++) {
        if(game->board[row_index + i][column_index + i] != player_marker) {
            player_streak = 0;
            continue;
        }

        if(++player_streak == NUMBER_TO_WIN)
            return 1;
    }

    player_streak = 0;
    decrement = MIN(MIN((NUMBER_OF_ROWS - 1) - row_index, column_index), NUMBER_TO_WIN - 1);
    increment = MIN(MIN((NUMBER_OF_COLUMNS - 1) - column_index, row_index), NUMBER_TO_WIN - 1);

    for(i = -decrement; i<=increment; i++) {
        if(game->board[row_index - i][column_index + i] != player_marker) {
            player_streak = 0;
            continue;
        }

        if(++player_streak == NUMBER_TO_WIN)
            return 1;
    }

    return 0;
}

/* --------------------GLOBAL FUNCTIONS--------------------*/

struct game_structure * game_init() {
    struct game_structure * game;
    game = malloc(sizeof(*game));

    if(!game)
        return NULL;

    game->player1_addr = malloc(sizeof(game->player1_addr));
    if(!game->player1_addr) {
        free(game);
        return NULL;
    }

    game->player2_addr = malloc(sizeof(game->player2_addr));
    if(!game->player2_addr) {
        free(game->player1_addr);
        free(game);
        return NULL;
    }

    game->username_player1[0] = '\0';
    game->username_player2[0] = '\0';

    game->username_player1[USERNAME_LENGTH] = '\0';
    game->username_player2[USERNAME_LENGTH] = '\0';

    game->current_player = 0;

    game->unconfirmed_requests = vector_allocate(sizeof(struct unconfirmed_request), (vec_it_dealloc_type)unconfirmed_request_deallocate);

    reset_game(game);
    return game;
}

int assign_player(struct game_structure * game, struct sockaddr_in * client_addr, char * buffer, int message_length) {
    if(!game || !client_addr || !buffer || message_length < 2)
        return 0;

    if(!game->username_player1[0]) {
        memcpy(game->player1_addr, client_addr, sizeof(*client_addr));
        strncpy(game->username_player1, buffer+1, MIN(message_length-1, USERNAME_LENGTH));
        return 1;
    }

    if(!game->username_player2[0]) {
        memcpy(game->player2_addr, client_addr, sizeof(*client_addr));
        memcpy(game->username_player2, buffer+1, MIN(message_length-1, USERNAME_LENGTH));
        return 2;
    }

    return 0;
}

void game_deallocate(struct game_structure ** game) {
    if(!game || !(*game))
        return;

    if((*game)->player1_addr) {
        free((*game)->player1_addr);
        (*game)->player1_addr = NULL;
    }
    
    if((*game)->player2_addr) {
        free((*game)->player2_addr);
        (*game)->player2_addr = NULL;
    }

    if((*game)->unconfirmed_requests) {
        vector_deallocate(&(*game)->unconfirmed_requests);
    }

    free(*game);
    *game = NULL;
}

int play_column(struct game_structure * game, int column) {
    int row;
    char player_marker;

    if(!game || column < 0 || column >= NUMBER_OF_COLUMNS)
        return 0;

    row = __get_empty_row(game, column);
    if(row == -1)
        return 0;

    player_marker = game->current_player%2 == 0 ? MARKER1 : MARKER2;

    game->board[row][column] = player_marker;

    return 1;
}

int check_tie(struct game_structure * game) {
    int i;

    for(i = 0; i<NUMBER_OF_COLUMNS; i++) {
        if(__get_empty_row(game, i) == -1)
            return 0;
    }

    return 1;
}

int check_win(struct game_structure * game, int column) {
    int row = __get_empty_row(game, column) + 1;
    char marker = game->current_player%2 ? MARKER1 : MARKER2;


    if(check_column(game, column, row, marker) || check_row(game, row, column, marker) || check_diagonals(game, row, column, marker))
        return 1;

    return 0;
}

