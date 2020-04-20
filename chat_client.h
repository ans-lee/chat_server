//
//  Contains functions that setups the chat client
//  and handles the client
//

/*
 *  Library Includes
 */

#include <netinet/in.h>

/*
 *  #defines
 */

#define DEFAULT_PORT    8080            // Setting this to 0 will pick a random port
#define MAX_USER_NAME   16
#define MSG_MAX         1000            // 1000 character limit for messages from the server and sending
#define IP_ADDR         "127.0.0.1"

/*
 *  Functions
 */

// Initialises the settings of the client and starts it up
static void setup_client(int *server_fd, struct sockaddr_in *server_address);

// Prompts the user for a username for the chat server
static char *prompt_for_username();

// Handles sending messages from the client to the server
static void *handle_send_message(void *data);

// Handles receiving messages from the server to the client
static void *handle_receive_message(void *data);
