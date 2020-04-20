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

/*
 *  Helper Function Prototypes
 */

void flush_stdin();

/*
 *  Main
 */

int main(void) {
    int server_fd;
    struct sockaddr_in server_address = {0};
    
    setup_client(&server_fd, &server_address);

    return EXIT_SUCCESS;
}

/*
 *  Functions
 */

static void setup_client(int *server_fd, struct sockaddr_in *server_address) {
    char *username = prompt_for_username();

    // Create the socket file descriptor
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup the socket
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(DEFAULT_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, IP_ADDR, &(server_address->sin_addr)) == 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Finally connect to the server
    if (connect(*server_fd, (struct sockaddr*) server_address, sizeof(*server_address)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Send username to the server to set this user's username
    send(*server_fd, username, strlen(username) + 1, 0);

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

static char *prompt_for_username() {
    char input[MAX_USER_NAME + 2];
    
    while (1) {
        printf("Enter a name (Max length is 16 characters): ");
        fgets(input, MAX_USER_NAME + 2, stdin);

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

static void *handle_send_message(void *data) {
    // Stop thread on return
    pthread_detach(pthread_self());

    // Collect arguments
    int server_fd = *((int *) data);

    char msg[MSG_MAX] = "";

    while (1) {
        fgets(msg, MSG_MAX, stdin);

        // Check if message is too long
        char *newline_char;
        if ((newline_char = strchr(msg, '\n')) == NULL) {
            printf("Your message exceeds the character limit.\n");
            flush_stdin();

            // Send newline to signal server to ignore this message
            send(server_fd, "\n", 2, 0);

            continue;
        }

        send(server_fd, msg, strlen(msg) + 1, 0);
    }

    // Destroy thread and return
    return NULL;
}

static void *handle_receive_message(void *data) {
    // Stop thread on return
    pthread_detach(pthread_self());

    // Collect arguments
    int server_fd = *((int *) data);

    char msg[MSG_MAX] = "";

    while (read(server_fd, msg, MSG_MAX) && strcmp(msg, "/quit") != 0) {
        printf("%s", msg);
    }

    printf("You have left the server.\n");

    // Destroy thread and return
    return NULL;
}

/*
 *  Helper Functions
 */

// Flushes standard input
void flush_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
