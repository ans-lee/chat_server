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

/*
 *  Non-library Includes
 */

#include "users.h"

/*
 *  #defines
 */

#define DEFAULT_PORT    8080
#define MAX_CONNS       10
#define MAX_USERS       20

/*
 *  Global Variables
 */

int n_users = 0;

/*
 *  Function Prototypes
 */

void setup_server(int *server_fd, struct sockaddr_in *server_address, int port_num);
void handle_connections(int *server_fd, struct sockaddr_in *client_address);

/*
 *  Main
 */

int main(void) {
    int server_fd;
    int conn_fd;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    setup_server(&server_fd, &server_address, DEFAULT_PORT);
    printf("######### Chat Server started on port %d\n\n", ntohs(server_address.sin_port));
    printf("--------- Chat Log\n");

    while (1) {
        handle_connections(&server_fd, &client_address);
    }

    return EXIT_SUCCESS;
}

/*
 *  Functions
 */

void setup_server(int *server_fd, struct sockaddr_in *server_address, int port_num) {
    // Create the socket file descriptor
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup the socket
    server_address->sin_family = AF_INET;
    server_address->sin_addr.s_addr = INADDR_ANY;
    server_address->sin_port = htons(port_num);

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

void handle_connections(int *server_fd, struct sockaddr_in *client_address) {
    int conn_fd;
    int addrlen = sizeof(client_address);

    // Create a socket to get the response from the connection
    if ((conn_fd = accept(*server_fd, (struct sockaddr*) client_address, (socklen_t*) &addrlen)) == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    /*
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, handle_client, NULL) < 0) {
        fprintf(stderr, "pthread_create: could not create thread\n");
    }
    */

    // Prevent users from joining the server if it's full
    if (n_users + 1 > MAX_USERS) {
        char *err_msg = "ServerError: The server is currently full\n";
        write(conn_fd, err_msg, strlen(err_msg));
        return;
    }

    // Setup and save the user's details
    struct user *user = malloc(sizeof(struct user));
    user->addr = *client_address;
    user->conn_fd = conn_fd;
    user->id = n_users;
    sprintf(user->name, "User %d", user->id);

    handle_user(user);
}
