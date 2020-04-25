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
    int conn_fd;
    int id;
    char name[MAX_USER_NAME + 1];   // +1 for NULL-terminating byte
};

/*
 *  Functions
 */

// Initialises users_mutex for global user variables
void init_users_mutex();

// Adds a user to the struct user global array
int add_user(struct user *user);

// Sends the message to all users
void send_message_to_all(struct user *user, char *message);

// Handles all communication with the user
void *handle_user(void *data);

// Creates a new user struct and returns the reference.
// Returns NULL if unsuccessful.
struct user *create_user(int conn_fd, char *username);

// Frees memory allocated to the user and close their connection
void destroy_user(struct user *user);

/*
 *  Server Message Functions
 */

// Prints to the server what thread ID the user connected on or failed to connect on
void server_print_user_join_status(struct user *user, int success);

// Prints to the server what thread ID the user left on
void server_print_user_left_status(struct user *user);
