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
    //printf("n_users = %d\n", n_users);
    //printf("id = %d\n", user->id);
    //printf("name = %s\n", user->name);

    char buffer[500] = {0};
    while (read(user->conn_fd, buffer, 500) > 0) {
        printf("%s: %s\n", user->name, buffer);
        bzero(buffer, 500);
    }
}
