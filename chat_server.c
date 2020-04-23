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
#include <pthread.h>
#include <errno.h>

/*
 *  Non-library Includes
 */

#include "chat_server.h"
#include "users.h"

/*
 *  Function Prototypes
 */

static void check_for_config_file();
static void create_config_file();
static void setup_server(int *server_fd, struct sockaddr_in *server_address);
static void handle_connections(int server_fd);

/*
 *  Main
 */

int main(void) {
    initialise_users_mutex();
    check_for_config_file();

    int server_fd;
    struct sockaddr_in server_address = {0};

    setup_server(&server_fd, &server_address);
    printf("######### Chat Server started on Port %d, listening on all Network Interfaces\n\n",
            ntohs(server_address.sin_port));
    printf("--------------------\n");
    printf("          Server Log\n");
    printf("--------------------\n");

    while (1) {
        handle_connections(server_fd);
    }

    return EXIT_SUCCESS;
}

/*
 *  Functions
 */

// Check for configuration file, if missing create one
static void check_for_config_file() {
    if (access(CONFIG_NAME, F_OK) == -1) {
        printf("Creating config file %s...\n", CONFIG_NAME);
        create_config_file();
    }

    if (access(CONFIG_NAME, R_OK) == -1) {
        fprintf(stderr, "error: %s must have read permissions", CONFIG_NAME);
        exit(EXIT_FAILURE);
    }
}

// Creates the configuration file
static void create_config_file() {
    // Create config file
    FILE *conf = fopen(CONFIG_NAME, "w");
    if (conf == NULL) {
        perror("fopen:");
        exit(EXIT_FAILURE);
    }

    // Write all text to the config file

    // Title comment
    fprintf(conf, "# Simple configuration file for server. Any line with # at the front will be ignored.\n\n");

    // Ports option
    fprintf(conf, "# Change ports here to adjust which ports the server should\n"
                  "# use, separated by ONE comma. DO NOT put anything after \"ports:\",\n"
                  "# change the numbers and use numbers greater than 0 and less than 65535 ONLY!\n"
                  "# e.g.\n"
                  "# ports:8080,7080,6080\n"
                  "# NOTE: ports below 2000 are usually reserved. Ports above 5000 should be fine.\n"
                  "ports:8080,7080,6080\n\n");

    fclose(conf);
}

// Read from the config file and set some variables
// TODO
/*
static void read_from_config_file() {
    FILE *conf = fopen(CONFIG_NAME, "r");
    if (conf == NULL) {
        perror("fopen:");
        exit(EXIT_FAILURE);
    }
}
*/

// Initialises the settings of the server and starts it up
static void setup_server(int *server_fd, struct sockaddr_in *server_address) {
    // Create the socket file descriptor
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup the socket
    server_address->sin_family = AF_INET;
    server_address->sin_addr.s_addr = INADDR_ANY;

    // Pick an available port
    int serv_ports[] = {SERV_PORT_1, SERV_PORT_2, SERV_PORT_3};
    int size = sizeof(serv_ports) / sizeof(serv_ports[0]);
    for (int i = 0; i < size; i++) {
        // Set the port
        server_address->sin_port = htons(serv_ports[i]);

        // Assign address to the socket
        if (bind(*server_fd, (struct sockaddr*) server_address, sizeof(*server_address)) == 0) {
            break;
        } else if (i == size - 1) {
            extern int errno;
            if (errno == EADDRINUSE) {
                fprintf(stderr, "bind: all available ports are occupied\n");
            } else {
                perror("bind");
            }
            exit(EXIT_FAILURE);
        }
    }

    // Tell socket to accept incoming connections
    if (listen(*server_fd, MAX_CONNS) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

// Handles incoming connections to the server
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
