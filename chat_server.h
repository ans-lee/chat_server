//
//  Contains functions that setups the chat server
//  and handles the server
//

/*
 *  Enums
 */

// Ports that the server can run on if not occupied
enum server_ports {
    SERV_PORT_1 = 8080,
    SERV_PORT_2 = 7080,
    SERV_PORT_3 = 6080
};

/*
 *  #defines
 */

#define CONFIG_NAME                     "config.txt"    // Configuration file name
#define MAX_CONNS                       10              // Max queing connections allowed
#define MAX_USERS                       20              // Max users allowed on the server at once
#define MSG_MAX                         1000            // 1000 character limit for messages

/*
 *  Structs
 */

struct serv_settings {
    int *ports;     // Array of Port numbers
};
