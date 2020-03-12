//
//  Contains functions that setups the chat server
//  and handles the server
//

/*
 *  Library Includes
 */

#include <netinet/in.h>

/*
 *  #defines
 */

#define DEFAULT_PORT    8080    // Setting this to 0 will pick a random port
#define MAX_CONNS       10      // Max queing connections allowed
#define MAX_USERS       20      // Max users allowed on the server at once
#define MSG_MAX         1000    // 1000 character limit for messages

/*
 *  Functions
 */

// Initialises the settings of the server and starts it up
static void setup_server(int *server_fd, struct sockaddr_in *server_address, int port_num);

// Handles incoming connections to the server
static void handle_connections(int *server_fd, struct sockaddr_in *client_address);
