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

static unsigned int n_users = 0;
static unsigned int next_user_id = 0;
static struct user *users[MAX_USERS];
static pthread_mutex_t users_mutex;

/*
 *  Functions
 */

void init_users_mutex() {
    int result = pthread_mutex_init(&users_mutex, NULL);
    if (result) {
        fprintf(stderr, "pthread_mutex_init: failed with error %d\n", result);
        exit(EXIT_FAILURE);
    }
}

int add_user(struct user *user) {
    pthread_mutex_lock(&users_mutex);
    if (n_users >= MAX_USERS) {
        pthread_mutex_unlock(&users_mutex);
        return -1;
    }

    int free_id = next_user_id;
    for (int i = 0; i < MAX_USERS; i++) {
        if (free_id >= MAX_USERS) {
            free_id = 0;
        }

        if (users[free_id] == NULL) {
            users[free_id] = user;
            user->id = free_id;
            next_user_id = free_id + 1;
            n_users++;
            pthread_mutex_unlock(&users_mutex);
            return 0;
        }
        free_id++;
    }

    // No free slots (The code should not get here)
    // n_users is incorrectly set if the code gets here
    pthread_mutex_unlock(&users_mutex);
    return -1;
}

void send_message_to_all(struct user *user, char *message) {
    char *buffer = malloc(strlen(user->name) + MSG_MAX);
    if (buffer == NULL) {
        fprintf(stderr, "send_message_to_all: not enough memory\n");
        return;
    }
    sprintf(buffer, "%s\t%s", user->name, message);

    // Print message to the server
    printf("[%s]: %s\n", user->name, message);

    pthread_mutex_lock(&users_mutex);
    int count = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i]) {
            count++;
            write(users[i]->conn_fd, buffer, strlen(buffer) + 1);
        }

        if (count == n_users) {
            break;
        }
    }
    pthread_mutex_unlock(&users_mutex);
    free(buffer);
}

void *handle_user(void *data) {
    // Stop thread on return
    pthread_detach(pthread_self());
    
    struct user *user = data;

    if (add_user(user)) {
        // Prevent users from joining the server if it's full
        server_print_user_join_status(user, 0);
        char *err_msg = "ServerError: The server is currently full\n";
        write(user->conn_fd, err_msg, strlen(err_msg) + 1);
        destroy_user(user);
        return NULL;
    } else {
        // Send message to the client to signal that it is connected
        server_print_user_join_status(user, 1);
        write(user->conn_fd, "", 1);
    }

    // Check for input from the user
    char buffer[MSG_MAX] = {0};
    while (read(user->conn_fd, buffer, MSG_MAX) > 0) {
        if (strcmp(buffer, "/quit") == 0) {
            write(user->conn_fd, "/quit", 6);
            break;
        }

        send_message_to_all(user, buffer);
        bzero(buffer, MSG_MAX);
    }

    server_print_user_left_status(user);

    pthread_mutex_lock(&users_mutex);
    users[user->id] = NULL;
    n_users--;
    pthread_mutex_unlock(&users_mutex);

    // Stop connection if no more input
    destroy_user(user);

    // Destroy thread and return
    return NULL;
}

struct user *create_user(int conn_fd, char *username) {
    struct user *user = malloc(sizeof(struct user));
    if (user == NULL) {
        return NULL;
    }

    user->conn_fd = conn_fd;
    user->id = -1;
    sprintf(user->name, "%s", username);

    return user;
}

void destroy_user(struct user *user) {
    close(user->conn_fd);
    free(user);
}

/*
 *  Server Message Functions
 */

void server_print_user_join_status(struct user *user, int success) {
    if (success) {
        printf("%s has joined\n", user->name);
    } else {
        printf("%s has failed to join\n", user->name);
    }
}

void server_print_user_left_status(struct user *user) {
    printf("%s has left\n", user->name);
}
