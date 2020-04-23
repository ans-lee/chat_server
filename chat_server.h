//
//  Contains functions that setups the chat server
//  and handles the server
//

/*
 *  #defines
 */

#define MAX_IPV4_LEN                    15          // Max length of an IPv4 ip address
#define DEFAULT_NET_INTERFACE_NAME      "eth0"      // Default network interface name to run the server on
#define DEFAULT_PORT                    8080        // Setting this to 0 will pick a random port
#define MAX_CONNS                       10          // Max queing connections allowed
#define MAX_USERS                       20          // Max users allowed on the server at once
#define MSG_MAX                         1000        // 1000 character limit for messages
