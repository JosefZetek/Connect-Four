#include "unconfirmed_request.h"

struct unconfirmed_request * unconfirmed_request_init(struct sockaddr_in * addr, char * message, int message_length) {
    struct unconfirmed_request * request;

    request = malloc(sizeof(*request));

    if(!request)
        return NULL;

    /* Copy address */
    request->addr = malloc(sizeof(*addr));
    if(!request->addr) {
        free(request);
        return NULL;
    }

    memcpy(request->addr, addr, sizeof(struct sockaddr));

    /* Copy message length and message contents */
    request->message_length = message_length;
    strncpy(request->message, message, message_length);

    request->timestamp = time(NULL);
    request->last_sent = 0;

    return request;
}

int compare_request(struct unconfirmed_request * request, struct sockaddr_in * addr, char message_type) {
    char request_message_type;
    socklen_t addr_len;

    if(!request || !addr)
        return 0;

    addr_len = sizeof(struct sockaddr_in);

    request_message_type = request->message[0];

    /* If not address */
    if(memcmp(request->addr, addr, addr_len))
        return 0;
    
    if(request_message_type != message_type)
        return 0;

    return 1;
}

void update_unconfirmed(int sockfd, struct unconfirmed_request * request) {

    time_t current_time;
    if(!request)
        return;

    current_time = time(NULL);

    /* If last sent time > TIMEOUT */
    if(difftime(current_time, request->last_sent) > TIMEOUT) {
        request->last_sent = time(NULL);
        send_message(sockfd, request->message, request->message_length, request->addr);
    }
}

void unconfirmed_request_deallocate(struct unconfirmed_request ** request) {
    if(!request || !(*request))
        return;

    if((*request)->addr) {
        free((*request)->addr);
        (*request)->addr = NULL;
    }

    free(*request);
    *request = NULL;
}
