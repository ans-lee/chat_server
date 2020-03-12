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

/*
 *  #defines
 */

#define MAX_USERS       20

/*
 *  Functions
 */

void *handle_user(void *data) {
    struct user *user = data;
    extern int n_users;

    // Prevent users from joining the server if it's full
    /*
    if (n_users + 1 > MAX_USERS) {
        char *err_msg = "ServerError: The server is currently full\n";
        write(user->conn_fd, err_msg, strlen(err_msg));
        return;
    }
    */

    n_users++;

    // Check for input from the user
    char buffer[MSG_MAX] = {0};
    while (read(user->conn_fd, buffer, MSG_MAX) > 0) {
        printf("%s: %s\n", user->name, buffer);
        bzero(buffer, MSG_MAX);
    }

    // Stop connection if no more input
    close(user->conn_fd);
    free(user);
    n_users--;

    // Destroy thread and return
    pthread_detach(pthread_self());
    return NULL;
}
