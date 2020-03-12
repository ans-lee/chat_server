//
//  Contains functions that handle the clients on
//  the chat server
//

/*
 *  Library Includes
 */

#include <netinet/in.h>

/*
 *  #defines
 */

#define MAX_USER_NAME   16      // 16 character limit for a user name

/*
 *  Structs
 */

struct user {
    struct sockaddr_in addr;
    int conn_fd;
    int id;
    char name[MAX_USER_NAME];
};

/*
 *  Functions
 */

// Handles all communication with the user
void *handle_user(void *data);

// Frees memory allocated to the user and close their connection
void destroy_user(struct user *user);
