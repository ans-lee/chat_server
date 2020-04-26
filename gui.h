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
void resize_gui();

// Reads from user input and stores input in the buffer, as
// well as drawing the input in the input box. Returns 1 if
// the message should be sent, 0 if not
int read_input_from_user(char *buff, int *pos, int length);

// Removes the gui and return to non curses mode
void destroy_gui();