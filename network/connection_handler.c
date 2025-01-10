#include "connection_handler.h"

static int clear_request(struct vector * games, struct game_structure * lobby, struct sockaddr_in * client_addr, char message_type) {
    struct game_structure * game;
    struct unconfirmed_request * request;
    int i, j;

    /* If searched request is inside games array */
    for(i = 0; i<vector_count(games); i++) {
        game = vector_at(games, i);

        for(j = 0; j<vector_count(game->unconfirmed_requests); j++) {
            request = vector_at(game->unconfirmed_requests, j);

            if(compare_request(request, client_addr, message_type)) {
                vector_remove(game->unconfirmed_requests, j);
                return 1;
            }
        }
    }

    if(!lobby)
        return 0;

    /* If searched request is inside lobby */
    for(i = 0; i<vector_count(lobby->unconfirmed_requests); i++) {
        request = vector_at(lobby->unconfirmed_requests, i);

        if(compare_request(request, client_addr, message_type)) {
            vector_remove(lobby->unconfirmed_requests, i);
            return 1;
        }
    }

    return 0;
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

    /* If only one player is registered, send him a message */
    if(players_assigned == 1) {
        /* Registered message creation */
        if(!registered_message(buffer, &message_length)) {
            printf("Failed to create registered message\n");
            return 0;
        }

        /* Registered message created */
        if(!(request = unconfirmed_request_init(client_addr, buffer, message_length))) {
            printf("Failed to initialize unconfirmed request\n");
            return 0;
        }

        /* Registered message added to queue */
        if(!vector_push_back((*game)->unconfirmed_requests, request)) {
            printf("Failed to add unconfirmed request to the list\n");
            return 0;
        }

        return 1;
    }
    
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

    return 1;
}

static int handle_confirmation(struct vector * games, struct game_structure * lobby, struct sockaddr_in * client_addr, char * buffer, int message_length) {
    struct game_structure * game;
    struct unconfirmed_request * request;
    int i, j;
    char confirmed_message_type;

    if(message_length != 2) {
        printf("Invalid confirmation message\n");
        return 0;
    }

    printf("Confirmation received\n");
    confirmed_message_type = buffer[1];
    printf("Message type: %d\n", confirmed_message_type);

    /* Check if the confirmation is for the lobby */
    clear_request(games, lobby, client_addr, confirmed_message_type);

    return 0;
}

void handle_message(int sockfd, struct vector * games, struct game_structure ** lobby) {
    struct sockaddr_in client_addr;
    char buffer[MTU];
    int length;
    socklen_t addr_len = sizeof(client_addr);

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
    }

    if(buffer[0] == CONFIRMATION_MESSAGE) {
        handle_confirmation(games, *lobby, &client_addr, buffer, length);
    }
}