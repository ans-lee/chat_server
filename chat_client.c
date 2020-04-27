//
//  Chat Client Program
//

/*
 *  Library Includes
 */

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

/*
 *  Non-library Includes
 */

#include "chat_client.h"
#include "messages.h"
#include "gui.h"

//TODO:
//- pad user's name to 16 chars for messages parsing or use \t
//  e.g. ABC             Hello
//- to fix race conditions, use a padded msg from server to
//  tell the client how big the message is - no need just bzero and send the msg

/*
 *  Function Prototypes
 */

static void setup_client(int *server_fd, struct sockaddr_in *server_address);
static char *prompt_for_username();
static void *handle_send_message(void *data);
static void *handle_receive_message(void *data);
static void flush_stdin();

/*
 *  Main
 */

int main(void) {
    init_msg_mutex();

    int server_fd;
    struct sockaddr_in server_address = {0};
    
    setup_client(&server_fd, &server_address);

    return EXIT_SUCCESS;
}

/*
 *  Functions
 */

// Initialises the settings of the client and starts it up
static void setup_client(int *server_fd, struct sockaddr_in *server_address) {
    char *username = prompt_for_username();

    // Create the socket file descriptor
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup the socket
    server_address->sin_family = AF_INET;

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, IP_ADDR, &(server_address->sin_addr)) == 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    int ports[MAX_PORTS] = {PORT_1, PORT_2, PORT_3};

    // Find which port the server is on
    for (int i = 0; i < MAX_PORTS; i++) {
        server_address->sin_port = htons(ports[i]);

        // Connect to the server with the given port and IP address
        if (connect(*server_fd, (struct sockaddr*) server_address, sizeof(*server_address)) == 0) {
            break;
        } else if (i == MAX_PORTS - 1) {
            // Connection failed on all ports
            fprintf(stderr, "Could not connect to the server. Either the "
                            "server is offline or your connection has issues.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Send username to the server to set this user's username
    send(*server_fd, username, strlen(username) + 1, 0);

    // Initialise the gui
    init_gui();

    // Create a thread for handling messages being sent
    pthread_t send_thread_id;

    if (pthread_create(&send_thread_id, NULL, handle_send_message, server_fd) < 0) {
        fprintf(stderr, "pthread_create: could not create handle_send_message thread\n");
        exit(EXIT_FAILURE);
    }

    // Create a thread for handling messages being received
    pthread_t receive_thread_id;

    if (pthread_create(&receive_thread_id, NULL, handle_receive_message, server_fd) < 0) {
        fprintf(stderr, "pthread_create: could not create handle_receive_message thread\n");
        exit(EXIT_FAILURE);
    }

    // Use pthread_join to allow main thread to wait for this thread before
    // returning in main()
    if (pthread_join(receive_thread_id, NULL)) {
        fprintf(stderr, "pthread_join: could not join handle_receive_message thread\n");
        exit(EXIT_FAILURE);
    }

    free(username);
}

// Prompts the user for a username for the chat server
static char *prompt_for_username() {
    char input[NAME_MAX + 2];
    
    while (1) {
        printf("Enter a name (Max length is 16 characters): ");
        fgets(input, NAME_MAX + 2, stdin);

        char *newline_char;
        if ((newline_char = strchr(input, '\n')) != NULL) {
            // Strip newline
            *newline_char = '\0';
            break;
        } else {
            printf("Name is longer than 16 characters, try again.\n");
            flush_stdin();
        }
    }

    char *username = strdup(input);
    if (username == NULL) {
        fprintf(stderr, "prompt_for_username: strdup failed\n");
        exit(EXIT_FAILURE);
    }

    return username;
}

// Handles sending messages from the client to the server
static void *handle_send_message(void *data) {
    // Stop thread on return
    pthread_detach(pthread_self());

    // Collect arguments
    int server_fd = *((int *) data);

    char msg[MSG_MAX] = {0};
    bzero(msg, MSG_MAX);

    int i = 0;
    while (1) {
        int send_msg = read_input_from_user(msg, &i, MSG_MAX);

        if (strcmp(msg, "/quit\n") == 0) {
            break;
        }

        if (send_msg) {
            send(server_fd, msg, strlen(msg) + 1, 0);
            bzero(msg, MSG_MAX);
        }

        resize_gui();
    }
    destroy_gui();
    exit(0);

    // Destroy thread and return
    return NULL;
}

// Handles receiving messages from the server to the client
static void *handle_receive_message(void *data) {
    // Stop thread on return
    pthread_detach(pthread_self());

    // Collect arguments
    int server_fd = *((int *) data);

    char buffer[MSG_MAX] = "";

    while (1) {
        if (recv(server_fd, buffer, MSG_MAX, 0) > 0) {
            if (strcmp(buffer, "/quit") == 0) {
                break;
            }

            if (strcmp(buffer, "") == 0) {
                continue;
            }

            char *msg_packet = strdup(buffer);
            char *sender = strtok(msg_packet, "\t");
            char *msg = strtok(NULL, "\t");

            struct message *new_msg = new_message(msg, sender);
            if (new_msg != NULL) {
                add_to_messages(new_msg);
            } else {
                fprintf(stderr, "Cannot store any more messages.\n");
            }
            free(msg_packet);
        }
        bzero(buffer, MSG_MAX);
    }
    destroy_gui();

    printf("You have left the server.\n");

    // Destroy thread and return
    return NULL;
}

// Flushes standard input
static void flush_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
