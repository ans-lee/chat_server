//
//  Contains all data related to client messages
//

#pragma once

/*
 *  #defines
 */

#define N_MSGS      10      // Max number of messages
#define MSG_MAX     1000    // Max length of a message
#define NAME_MAX    16      // Max length of a name

/*
 *  Structs
 */

// Stores all the messages using a linked list and the number
// of messages
struct message {
    char content[MSG_MAX];
    char sender[NAME_MAX];
};

/*
 *  Functions
 */

// Initialises msg_mutex for global message variables
void init_msg_mutex();

// Allocate space for a new message struct and return it. If sender is NULL,
// the sender string is left empty. Returns NULL if failed.
struct message *new_message(char *msg, char *sender);

// Adds a message to the global msgs buffer
void add_to_messages(struct message *msg);

// Frees memory allocated to the struct message passed in
void destroy_message(struct message *msg);
