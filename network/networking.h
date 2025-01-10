#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include "../data_structures/vector.h"
#include "../constants.h"

struct socket_connection {
    int sockfd;
    struct sockaddr_in server_addr;
};

/**
 * @brief Initialize a new socket connection
 * @param connection The socket connection to initialize
 * @return Returns 1 if the initialization was successful, 0 otherwise
 */
int init_socket(struct socket_connection * connection);

/**
 * @brief Function receives a message from a client
 * and stores it in a buffer along with the length that is stored
 * @param sockfd The socket file descriptor
 * @param length The length of the received message
 * @param buffer The buffer to store the message in
 * @param client_addr The address of the client that sent the message
 * @return Returns 1 if the message was received successfully, 0 otherwise
 */
int receive_message(int sockfd, int * length, char * buffer, struct sockaddr_in * client_addr, socklen_t * addr_len);

/**
 * @brief Function sends a message to a client
 * @param sockfd The socket file descriptor
 * @param buffer The buffer containing the message
 * @param length The length of the message
 * @param client_addr The address of the client to send the message to
 * @return Returns 1 if the message was sent successfully, 0 otherwise
 */
int send_message(int sockfd, char * buffer, int length, struct sockaddr_in * client_addr);

