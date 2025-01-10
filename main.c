#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>


#include "data_structures/game_structure.h"
#include "data_structures/vector.h"
#include "constants.h"

#include "network/networking.h"
#include "network/message_formatter.h"
#include "network/connection_handler.h"

#define UDP_WAIT_SECONDS 0
#define UDP_WAIT_MICROSECONDS 100000

static int get_timeout(struct timeval * timeout) {
	if(!timeout)
		return 0;

	timeout->tv_sec = UDP_WAIT_SECONDS;
	timeout->tv_usec = UDP_WAIT_MICROSECONDS;
	return 1;
}

static struct game_structure * get_game(struct vector * games, struct sockaddr_in * client_addr) {
    struct game_structure * game;
    int i;

    for(i = 0; i<vector_count(games); i++) {
        game = vector_at(games, i);

        if(!memcmp(game->player1_addr, client_addr, sizeof(struct sockaddr_in)))
            return game;

        if(!memcmp(game->player2_addr, client_addr, sizeof(struct sockaddr_in)))
            return game;
    }

    return NULL;
}

static void resend_unconfirmed_messages(int sockfd, struct vector * games, struct game_structure * lobby) {
    struct game_structure * game;
    struct unconfirmed_request * request;
    int i, j;
    
    /* Send unconfirmed messages */
    for(i = 0; i<vector_count(games); i++) {
        game = vector_at(games, i);

        if(!vector_count(game->unconfirmed_requests))
            continue;

        request = vector_at(game->unconfirmed_requests, j);
        update_unconfirmed(sockfd, request);
    }
}

int main() {
	struct socket_connection connection;
	struct game_structure * lobby = NULL;
	struct vector * games = NULL, * unconfirmed_requests = NULL;
    struct timeval timeout;
    fd_set readfds;

    if(!init_socket(&connection))
		return EXIT_FAILURE;

	if(!get_timeout(&timeout)) {
		close(connection.sockfd);
		return EXIT_FAILURE;
	}

	if(!(games = vector_allocate(sizeof(struct game_structure), (vec_it_dealloc_type)game_deallocate))) {
		close(connection.sockfd);
		return EXIT_FAILURE;
	}

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(connection.sockfd, &readfds);

        // Use select() to block until there is data to read or timeout
        int activity = select(connection.sockfd + 1, &readfds, NULL, NULL, &timeout);

        if (activity < 0) {
            printf("Select error\n");
            continue;
        }

        // If there is data available on the UDP socket, handle it
        if (FD_ISSET(connection.sockfd, &readfds)) {
            handle_message(connection.sockfd, games, &lobby);
        }

        // Check for inactivity (disconnected clients)
        // check_disconnects();

        resend_unconfirmed_messages(connection.sockfd, games, lobby);
    }

    close(connection.sockfd);
    return EXIT_SUCCESS;
}
