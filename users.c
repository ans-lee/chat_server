/*
 *  Library Includes
 */

#include <stdio.h>
#include <unistd.h>
#include <strings.h>

/*
 *  Non-library Includes
 */

#include "users.h"

/*
 *  Functions
 */

void handle_user(struct user *user) {
    extern int n_users;
    n_users++;

    char buffer[MSG_MAX] = {0};
    while (read(user->conn_fd, buffer, MSG_MAX) > 0) {
        printf("%s: %s\n", user->name, buffer);
        bzero(buffer, MSG_MAX);
    }
}
