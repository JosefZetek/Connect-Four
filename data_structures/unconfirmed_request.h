#pragma once

#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>

#include "../network/networking.h"

/* Number of seconds before request is sent again */
#define TIMEOUT 5

/**
 * @brief Structure to store unconfirmed requests
 * @param addr The address of the client that sent the request
 * @param message The message that was sent
 * @param message_length The length of the message
 * @param timestamp The time the request was sent
 */
struct unconfirmed_request {
    struct sockaddr_in * addr;
    char message[MTU];
    int message_length;
    time_t timestamp;
    time_t last_sent;
};

/**
 * @brief Initialize a new unconfirmed request structure
 * @param addr The address of the client that sent the request
 * @param message The message that was sent
 * @param message_length The length of the message
 * @return Returns a pointer to the new unconfirmed request structure
 */
struct unconfirmed_request * unconfirmed_request_init(struct sockaddr_in * addr, char * message, int message_length);

/**
 * @brief Frees memory allocated for the unconfirmed request structure and marks the pointer as NULL
 * @param request Pointer to pointer to the unconfirmed request structure to deallocate
 */
void unconfirmed_request_deallocate(struct unconfirmed_request ** request);

/**
 * @brief Resent the unconfirmed request if last sent time > TIMEOUT
 * @param sockfd The socket file descriptor
 * @param request The unconfirmed request structure to update
 */
void update_unconfirmed(int sockfd, struct unconfirmed_request * request);

/**
 * @brief Compares if a given request is the same as the one provided
 * @param request The unconfirmed request structure to compare
 * @param addr The address of the client that sent the request
 * @param message_type The type of message that was sent
 * @return Returns 1 if the requests are the same, 0 otherwise
 */
int compare_request(struct unconfirmed_request * request, struct sockaddr_in * addr, char message_type);