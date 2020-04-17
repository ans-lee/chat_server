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
    handle_send_message(server_fd);

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

static void handle_send_message(int server_fd) {
    char buffer[MSG_MAX] = {0};

    while (read(server_fd, buffer, MSG_MAX) && strcmp(buffer, "/quit") != 0) {
        printf("%s", buffer);

        char msg[MSG_MAX] = "";
        fgets(msg, MSG_MAX, stdin);

        char *newline_char;
        if ((newline_char = strchr(msg, '\n')) == NULL) {
            printf("Your message exceeds the character limit.\n");
            flush_stdin();
            send(server_fd, "\n", 2, 0);
            continue;
        }

        send(server_fd, msg, strlen(msg) + 1, 0);
        if (strcmp(msg, "/quit") == 0) {
            break;
        }
    }
    printf("You have left the server.\n");
}

/*
 *  Helper Functions
 */

// Flushes standard input
void flush_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
