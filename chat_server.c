//
//  Chat Server Program
//

/*
 *  Library Includes
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

/*
 *  Non-library Includes
 */

#include "chat_server.h"
#include "users.h"

/*
 *  Main
 */

int main(void) {
    initialise_users_mutex();

    int server_fd;
    struct sockaddr_in server_address = {0};

    setup_server(&server_fd, &server_address);
    printf("######### Chat Server started on port %d\n\n", ntohs(server_address.sin_port));
    printf("--------- Server Log\n");

    while (1) {
        handle_connections(server_fd);
    }

    return EXIT_SUCCESS;
}

/*
 *  Functions
 */

static void setup_server(int *server_fd, struct sockaddr_in *server_address) {
    // Create the socket file descriptor
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup the socket
    server_address->sin_family = AF_INET;
    server_address->sin_addr.s_addr = INADDR_ANY;
    server_address->sin_port = htons(DEFAULT_PORT);

    // Assign address to the socket
    if (bind(*server_fd, (struct sockaddr*) server_address, sizeof(*server_address)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Tell socket to accept incoming connections
    if (listen(*server_fd, MAX_CONNS) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

static void handle_connections(int server_fd) {
    int conn_fd;
    struct sockaddr_in client_address = {0};
    int addrlen = sizeof(client_address);

    // Create a socket to get the response from the connection
    if ((conn_fd = accept(server_fd, (struct sockaddr*) &client_address, (socklen_t*) &addrlen)) == -1) {
        perror("accept");
        return;
    }

    char username[MAX_USER_NAME + 1] = {0};    // +1 for NULL-terminating byte
    if (read(conn_fd, username, MAX_USER_NAME + 1) < 1) {
        fprintf(stderr, "read: failed to read username from the client\n");
        return;
    }
    
    struct user *user = create_user(conn_fd, username);
    if (user == NULL) {
        fprintf(stderr, "create_user: not enough memory\n");
        close(conn_fd);
        return;
    }

    // Create a thread for the new user
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, handle_user, user) < 0) {
        fprintf(stderr, "pthread_create: could not create thread\n");
        destroy_user(user);
    }
}
