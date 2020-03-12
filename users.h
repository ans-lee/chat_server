//
//  Contains functions that handle the clients on
//  the chat server
//

/*
 *  Includes
 */

#include <netinet/in.h>

/*
 *  #defines
 */

#define MSG_MAX     1000    // 1000 character limit for messages

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
void *handle_user(void *data);

// Frees memory allocated to the user and close their connection
void destroy_user(struct user *user);
