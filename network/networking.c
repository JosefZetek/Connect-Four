#include "networking.h"

int init_socket(struct socket_connection * connection) {
    if(!connection)
        return 0;

    /* Create UDP socket */ 
    connection->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (connection->sockfd < 0) {
		printf("Failed to create socket\n");
        return 0;
    }

    /* Bind socket to a port */
    memset(&connection->server_addr, 0, sizeof(connection->server_addr));
    connection->server_addr.sin_family = AF_INET;
    connection->server_addr.sin_addr.s_addr = INADDR_ANY;
    connection->server_addr.sin_port = htons(PORT);

    if (bind(connection->sockfd, (struct sockaddr *)&connection->server_addr, sizeof(connection->server_addr)) < 0) {
        printf("Bind failed\n");
        return 0;
    }

    return 1; 
}

int receive_message(int sockfd, int * length, char * buffer, struct sockaddr_in * client_addr, socklen_t * addr_len) {
    if(!buffer || !length || !client_addr)
        return 0;
    
    memset(buffer, 0, MTU);  // Use MTU instead of sizeof(buffer)
    *length = recvfrom(sockfd, buffer, MTU, 0, (struct sockaddr *)client_addr, addr_len);
    return (*length > 0);
}

int send_message(int sockfd, char * buffer, int length, struct sockaddr_in * client_addr) {
    if(!buffer || !client_addr)
        return 0;
    
    socklen_t addr_len = sizeof(struct sockaddr_in);
    return sendto(sockfd, buffer, length, 0, (struct sockaddr *)client_addr, addr_len) == length;
}
