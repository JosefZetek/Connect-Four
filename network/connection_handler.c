#include "connection_handler.h"

static struct game_structure * get_game(struct vector * games, struct game_structure * lobby, struct sockaddr_in * client_addr) {
    struct game_structure * game;
    int i;

    socklen_t client_size = sizeof(struct sockaddr_in);

    for(i = 0; i<vector_count(games); i++) {
        game = vector_at(games, i);

        if(game == NULL)
            continue;

        if(memcmp(game->player1_addr, client_addr, client_size) || memcmp(game->player2_addr, client_addr, client_size))
            return game;
    }

    if(lobby && (memcmp(lobby->player1_addr, client_addr, client_size) || memcmp(lobby->player2_addr, client_addr, client_size)))
        return lobby;

    return NULL;
}

static int clear_request(struct game_structure * game, struct sockaddr_in * client_addr, char message_type) {
    struct unconfirmed_request * request;

    printf("OK");
    /* If no confirmation is required */
    if(!vector_count(game->unconfirmed_requests))
        return 0;
    
    printf("OK2");
    request = vector_at(game->unconfirmed_requests, 0);
    
    
    /* If the confirmed different request than wanted */
    if(!compare_request(request, client_addr, message_type))
        return 0;

    printf("OK3");
    /* Remove confirmed request */
    vector_remove(game->unconfirmed_requests, 0);
    return 1;
}


static int handle_connect(struct vector * games, struct game_structure ** game, struct sockaddr_in * client_addr, char * buffer, int message_length) {

    struct unconfirmed_request * request;
    struct game_structure * game_ptr;
    int players_assigned = 0;

    if(!(*game) && !(*game = game_init())) {
        printf("Failed to initialize game\n");
        return 0;
    }

    players_assigned = assign_player(*game, client_addr, buffer, message_length);

    /* Couldnt assign player */
    if(!players_assigned) {
        printf("Failed to assign player to the game\n");
        return 0;
    }

    /* If only one player is assigned to the game, just return */
    if(players_assigned == 1)
        return 1;
    
    /* If both players are assigned to the game, add the game to the list */
    if(!vector_push_back(games, *game)) {
        printf("Failed to add game to the list\n");
        return 0;
    }

    game_ptr = vector_at(games, vector_count(games) - 1);

    (*game) = NULL;

    /* Create start game message for player 1 */
    if(!start_game_message(buffer, &message_length, 0)) {
        printf("Failed to create start game message\n");
        vector_remove(game_ptr->unconfirmed_requests, vector_count(game_ptr->unconfirmed_requests) - 1);
        return 0;
    }

    if(!(request = unconfirmed_request_init(game_ptr->player1_addr, buffer, message_length))) {
        printf("Failed to initialize unconfirmed request\n");
        return 0;
    }

    if(!vector_push_back(game_ptr->unconfirmed_requests, request)) {
        printf("Failed to add unconfirmed request to the list\n");
        return 0;
    }

    /* Create start game message for player 2 */
    if(!start_game_message(buffer, &message_length, 1)) {
        printf("Failed to create start game message\n");
        return 0;
    }

    if(!(request = unconfirmed_request_init(game_ptr->player2_addr, buffer, message_length))) {
        printf("Failed to initialize unconfirmed request\n");
        return 0;
    }

    if(!vector_push_back(game_ptr->unconfirmed_requests, request)) {
        printf("Failed to add unconfirmed request to the list\n");
        return 0;
    }

    /* Create turn game message */
    if(!turn_message(buffer, &message_length, 0)) {
        printf("Failed to create turn message\n");
        return 0;
    }

    if(!(request = unconfirmed_request_init(game_ptr->player1_addr, buffer, message_length))) {
        printf("Failed to initialize unconfirmed request\n");
        return 0;
    }

    if(!vector_push_back(game_ptr->unconfirmed_requests, request)) {
        printf("Failed to add unconfirmed request to the list\n");
        return 0;
    }

    return 1;
}

static int handle_confirmation(struct game_structure * game, struct sockaddr_in * client_addr, char * buffer, int message_length) {
    struct unconfirmed_request * request;
    int i, j;
    char confirmed_message_type;

    if(message_length != 2) {
        printf("Invalid confirmation message\n");
        return 0;
    }

    confirmed_message_type = buffer[1];
    printf("Confirmation of message type: %d\n", confirmed_message_type);

    /* Check if the confirmation is for the lobby */
    if(!clear_request(game, client_addr, confirmed_message_type)) {
        printf("Failed to clear request\n");
        return 0;
    }

    if(confirmed_message_type == TURN_MESSAGE) {
        printf("Start game message confirmed\n");
        /* timeout should start */
        return 1;
    }

    return 1;
}

static int handle_play(struct game_structure * game, struct sockaddr_in * client_addr, char * buffer, int message_length) {

    struct unconfirmed_request * request;
    struct sockaddr_in * current_player, * opponent_player;

    socklen_t client_size = sizeof(*client_addr);
    int column;

    if(message_length != 2) {
        printf("Invalid play message\n");
        return 0;
    }

    if(memcmp(game->player1_addr, client_addr, client_size) && game->current_player != 0) {
        printf("It is not players turn\n");
        return 0;
    }

    if(memcmp(game->player2_addr, client_addr, client_size) && game->current_player != 1) {
        printf("It is not players turn\n");
        return 0;
    }


    if(game->current_player%2) {
        current_player = game->player2_addr;
        opponent_player = game->player1_addr;
    }
    else {
        current_player = game->player1_addr;
        opponent_player = game->player2_addr;
    }

    if(vector_count(game->unconfirmed_requests)) {
        printf("Unconfirmed requests are still pending\n");
        return 0;
    }

    column = buffer[1];

    if(!play_column(game, column)) {
        printf("Failed to play column\n");
        return 0;
    }

    /* Create state message */
    if(!state_message(buffer, &message_length, game, column)) {
        printf("Failed to create state message\n");
        return 0;
    }

    /* Add state message to first player */
    if(!(request = unconfirmed_request_init(game->player1_addr, buffer, message_length))) {
        printf("Failed to initialize unconfirmed request\n");
        return 0;
    }

    if(!vector_push_back(game->unconfirmed_requests, request)) {
        printf("Failed to add unconfirmed request to the list\n");
        return 0;
    }

    /* Add state message to second player */
    if(!(request = unconfirmed_request_init(game->player2_addr, buffer, message_length))) {
        printf("Failed to initialize unconfirmed request\n");
        return 0;
    }

    if(!vector_push_back(game->unconfirmed_requests, request)) {
        printf("Failed to add unconfirmed request to the list\n");
        return 0;
    }

    if(check_tie(game)) {
        printf("Game is a tie\n");

        if(!end_game_message(buffer, &message_length, TIE)) {
            printf("Failed to create end game message");
            return 0;
        }

        if(!(request = unconfirmed_request_init(game->player1_addr, buffer, message_length))) {
            printf("Failed to initialize unconfirmed request\n");
            return 0;
        }

        if(!vector_push_back(game->unconfirmed_requests, request)) {
            printf("Failed to add unconfirmed request\n");
            return 0;
        }

        if(!(request = unconfirmed_request_init(game->player2_addr, buffer, message_length))) {
            printf("Failed to initialize unconfirmed request\n");
            return 0;
        }

        if(!vector_push_back(game->unconfirmed_requests, request)) {
            printf("Failed to add unconfirmed request\n");
            return 0;
        }

        return 1;
    }

    if(check_win(game, column)) {
        printf("Game is won by the current player\n");

        if(!end_game_message(buffer, &message_length, WIN)) {
            printf("Failed to create end game message\n");
            return 0;
        }

        if(!(request = unconfirmed_request_init(current_player, buffer, message_length))) {
            printf("Failed to initialize unconfirmed request\n");
            return 0;
        }

        if(!vector_push_back(game->unconfirmed_requests, request)) {
            printf("Failed to add unconfirmed request\n");
            return 0;
        }

        if(!end_game_message(buffer, &message_length, LOSE)) {
            printf("Failed to create end game message\n");
            return 0;
        }


        if(!(request = unconfirmed_request_init(opponent_player, buffer, message_length))) {
            printf("Failed to initialize unconfirmed request\n");
            return 0;
        }

        if(!vector_push_back(game->unconfirmed_requests, request)) {
            printf("Failed to add unconfirmed request\n");
            return 0;
        }

        return 1;
    }

    if(!state_message(buffer, &message_length, game, column)) {
        printf("Failed to create state message\n");
        return 0;
    }

    if(!(request = unconfirmed_request_init(current_player, buffer, message_length))) {
        printf("Failed to initialize unconfirmed request\n");
        return 0;
    }

    if(!vector_push_back(game->unconfirmed_requests, request)) {
        printf("Failed to add unconfirmed request\n");
        return 0;
    }

    if(!(request = unconfirmed_request_init(opponent_player, buffer, message_length))) {
        printf("Failed to initialize unconfirmed request\n");
        return 0;
    }

    if(!vector_push_back(game->unconfirmed_requests, request)) {
        printf("Failed to add unconfirmed request\n");
        return 0;
    }

    if(!turn_message(buffer, &message_length, game->current_player)) {
        printf("Failed to create turn message\n");
        return 0;
    }

    if(!(request = unconfirmed_request_init(opponent_player, buffer, message_length))) {
        printf("Failed to initialize unconfirmed request\n");
        return 0;
    }

    if(!vector_push_back(game->unconfirmed_requests, request)) {
        printf("Failed to add unconfirmed request\n");
        return 0;
    }

    game->current_player = (game->current_player+1)%2;
    return 1;
}

void handle_message(int sockfd, struct vector * games, struct game_structure ** lobby) {
    char buffer[MTU];
    int length;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct game_structure * game;

    /* If either games or lobby is NULL, return */
    if(!games || !lobby)
        return;
    
    /* Initialize the client_addr structure */
    memset(&client_addr, 0, sizeof(client_addr));

    printf("Message received\n");

    // Modify receive_message to use addr_len
    if(!receive_message(sockfd, &length, buffer, &client_addr, &addr_len))
        return;
        
    
    if(buffer[0] == CONNECT_MESSAGE) {
        handle_connect(games, lobby, &client_addr, buffer, length);
        return;
    }

    /* Gets the game where this player plays*/
    game = get_game(games, *lobby, &client_addr);
    if(!game)
        return;

    if(buffer[0] == CONFIRMATION_MESSAGE) {
        handle_confirmation(game, &client_addr, buffer, length);
        return;
    }

    if(buffer[0] == PLAY_MESSAGE) {

        handle_play(game, &client_addr, buffer, length);
        printf("Play message received\n");
        return;
    }
}

