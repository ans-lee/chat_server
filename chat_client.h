//
//  Contains functions that setups the chat client
//  and handles the client
//

/*
 *  Enums
 */

// Ports the client will attempt to connect to server
enum server_ports {
    PORT_1 = 8080,
    PORT_2 = 7080,
    PORT_3 = 6080,
};

/*
 *  #defines
 */

#define IP_ADDR         "127.0.0.1"     // IP address of the server the client connects to. Change this to the server's IP
#define MAX_PORTS       3               // Max number of ports the client will attempt to connect to
