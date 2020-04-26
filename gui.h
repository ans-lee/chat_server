//
//  Contains functions that setups the gui
//  on the client
//

#pragma once

/*
 *  Non-library Includes
 */

#include "messages.h"

/*
 *  #defines
 */

#define CHAT_BOX_SIZE   3   // Size of the chat box

/*
 *  Functions
 */

// Initialises the gui on the console screen
void init_gui();

// Checks if gui needs to be resized and resizes if needed
void handle_gui_resize();

// Adds a message to the chat box given the message struct
void add_msg_to_chat_box(struct message *msg);

// Removes the gui and return to non curses mode
void destroy_gui();
