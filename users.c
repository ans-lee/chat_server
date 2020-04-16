/*
 *  Library Includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

/*
 *  Non-library Includes
 */

#include "users.h"
#include "chat_server.h"

/*
 *  Global Variables
 */

unsigned int n_users = 0;
struct user *users[MAX_USERS];
pthread_mutex_t users_mutex = {0};

/*
 *  Functions
 */

void initialise_users_mutex() {
    int result = pthread_mutex_init(&users_mutex, NULL);
    if (result) {
        fprintf(stderr, "pthread_mutex_init: failed with error %d\n", result);
        exit(EXIT_FAILURE);
    }
}

unsigned int get_n_users() {
    pthread_mutex_lock(&users_mutex);
    int result = n_users;
    pthread_mutex_unlock(&users_mutex);

    return n_users;
}

void *handle_user(void *data) {
    // Stop thread on return
    pthread_detach(pthread_self());
    
    struct user *user = data;

    if (n_users < MAX_USERS) {
        // Send message to the client to signal that it is connected
        write(user->conn_fd, "Keep-Alive", 10);
    } else {
        // Prevent users from joining the server if it's full
        char *err_msg = "ServerError: The server is currently full\n";
        write(user->conn_fd, err_msg, strlen(err_msg) + 1);
        destroy_user(user);
        return NULL;
    }
    n_users++;

    // Check for input from the user
    char buffer[MSG_MAX] = {0};
    while (read(user->conn_fd, buffer, MSG_MAX) > 0) {
        if (strcmp(buffer, "/quit\n") == 0) {
            char *leave_msg = "You have left the server.\n";
            write(user->conn_fd, leave_msg, strlen(leave_msg) + 1);
            break;
        }

        printf("%s: %s\n", user->name, buffer);
        //TODO: save messages to the server and transmit them to other users
        bzero(buffer, MSG_MAX);
        
        // Send message to the client to signal that it is still connected
        write(user->conn_fd, "Keep-Alive", 11);
    }

    // Stop connection if no more input
    destroy_user(user);
    n_users--;

    // Destroy thread and return
    return NULL;
}

struct user *create_user(struct sockaddr_in *client_address, int conn_fd) {
    struct user *user = malloc(sizeof(struct user));
    if (user == NULL) {
        return NULL;
    }

    user->addr = client_address;
    user->conn_fd = conn_fd;

    pthread_mutex_lock(&users_mutex);
    user->id = n_users;
    pthread_mutex_unlock(&users_mutex);

    sprintf(user->name, "User %d", user->id);

    return user;
}

void destroy_user(struct user *user) {
    close(user->conn_fd);
    free(user->addr);
    free(user);
}
