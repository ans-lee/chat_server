//
//  Contains functions that handle the clients on
//  the chat server
//

/*
 *  Includes
 */

#include <netinet/in.h>

/*
 *  Structs
 */

struct user {
    struct sockaddr_in addr;
    int conn_fd;
    int id;
    char name[16];
};

/*
 *  Functions
 */

// Handles all communication with the user
void handle_user(struct user *user);
