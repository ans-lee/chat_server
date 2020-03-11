/*
 *  Library Includes
 */

#include <stdio.h>

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
    printf("n_users = %d\n", n_users);
}
