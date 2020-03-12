/*
 *  Library Includes
 */

#include <stdio.h>
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

void handle_user(struct user *user) {
    extern int n_users;

    // Prevent users from joining the server if it's full
    if (n_users + 1 > MAX_USERS) {
        char *err_msg = "ServerError: The server is currently full\n";
        write(user->conn_fd, err_msg, strlen(err_msg));
        return;
    }

    n_users++;

    char buffer[MSG_MAX] = {0};
    while (read(user->conn_fd, buffer, MSG_MAX) > 0) {
        printf("%s: %s\n", user->name, buffer);
        bzero(buffer, MSG_MAX);
    }
}
