//
//  Contains functions that setups the chat server
//  and handles the server
//

#pragma once

/*
 *  #defines
 */

#define CONFIG_NAME     "config.txt"    // Configuration file name
#define MAX_PORTS       5               // Number of ports the server is allowed to run on
#define DEFAULT_PORT    8080            // Default port in case config ports contain nothing
#define MAX_CONNS       10              // Max queing connections allowed
#define MAX_USERS       20              // Max users allowed on the server at once
#define MSG_MAX         1000            // 1000 character limit for messages

/*
 *  Structs
 */

// Stores the settings of the server. This struct will filled in according
// to the settings in config.txt or default values will be set if the fields
// are not found
struct server_config {
    int ports[MAX_PORTS];     // Array of Port numbers
};
