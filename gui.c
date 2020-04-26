/*
 *  Library Includes
 */

#include <ncurses.h>
#include <unistd.h>
#include <string.h>

/*
 *  Non-library Includes
 */

#include "gui.h"
#include "messages.h"

/*
 *  Function Prototypes
 */

static void create_chat_win();
static void create_chat_box();
static void create_input_win();
static void create_input_box();
static void draw_borders(WINDOW *screen);

/*
 *  Global Variables
 */

static WINDOW *chat_win;
static WINDOW *chat_box;
static WINDOW *input_win;
static WINDOW *input_box;
int max_x, max_y;           // Console screen dimensions

/*
 *  Public Functions
 */

void init_gui() {
    // Initialise ncurses settings for the window
    initscr();
    noecho();
    curs_set(FALSE);

    // Get console window size into the variables
    getmaxyx(stdscr, max_y, max_x);

    // Make all windows
    create_chat_win();
    create_input_win();
    create_chat_box();
    create_input_box();

    // Check everytime a resize is needed
    while (1) {
        handle_gui_resize();
    }

    endwin();
}

void handle_gui_resize() {
    // Get new console screen dimensions
    int new_y;
    int new_x;
    getmaxyx(stdscr, new_y, new_x);

    if (new_y == max_y && new_x == max_x) {
        // No need to resize, stop this function
        // However, all windows still need to be refreshed
        wrefresh(chat_win);
        wrefresh(chat_box);
        wrefresh(input_win);
        wrefresh(input_box);
        return;
    }

    // Reset global variables to hold the current console screen size
    max_x = new_x;
    max_y = new_y;

    // Resize and reposition all windows
    wresize(chat_win, new_y - CHAT_BOX_SIZE, new_x);
    wresize(chat_box, new_y - CHAT_BOX_SIZE - 2, new_x - 2);
    wresize(input_win, CHAT_BOX_SIZE, new_x);
    mvwin(input_win, new_y - CHAT_BOX_SIZE, 0);
    wresize(input_box, 1, new_x - 2);
    mvwin(input_box, new_y - CHAT_BOX_SIZE + 1, 1);

    // Clear windows
    wclear(stdscr);
    wclear(chat_win);
    wclear(chat_box);
    wclear(input_win);
    wclear(input_box);

    // Redraw all borders
    draw_borders(chat_win);
    draw_borders(input_win);

    // Redraw messages on windows
    mvwprintw(chat_win, 0, 2, "Chat");
    mvwprintw(input_box, 0, 0, "Start typing to chat...");

    // Refresh all windows
    wrefresh(chat_win);
    wrefresh(chat_box);
    wrefresh(input_win);
    wrefresh(input_box);
}

void add_msg_to_chat_box(struct message *msg) {
    if (strlen(msg->sender) == 0) {
        wprintw(chat_box, "[SERVER]: %s", msg->sender, msg->msg);
    } else {
        wprintw(chat_box, "[%s]: %s", msg->sender, msg->msg);
    }
}

void destroy_gui() {
    endwin();
}

/*
 *  Private Functions
 */

// Creates the chat window
static void create_chat_win() {
    getmaxyx(stdscr, max_y, max_x);

    chat_win = newwin(max_y - CHAT_BOX_SIZE, max_x, 0, 0);
    draw_borders(chat_win);
    mvwprintw(chat_win, 0, 2, "Chat");

    wrefresh(chat_win);
}

// Creates the chat box window, used to store chat messages
static void create_chat_box() {
    getmaxyx(stdscr, max_y, max_x);

    chat_box = newwin(max_y - CHAT_BOX_SIZE - 2, max_x - 2, 1, 1);

    wrefresh(chat_box);
}

// Creates the input window
static void create_input_win() {
    getmaxyx(stdscr, max_y, max_x);

    input_win = newwin(CHAT_BOX_SIZE, max_x, max_y - CHAT_BOX_SIZE, 0);
    draw_borders(input_win);

    wrefresh(input_win);
}

// Creates the input box window, used to store the user's input message
static void create_input_box() {
    getmaxyx(stdscr, max_y, max_x);

    input_box = newwin(1, max_x - 2, max_y - CHAT_BOX_SIZE + 1, 1);
    mvwprintw(input_box, 0, 0, "Start typing to chat...");

    wrefresh(input_box);
}

// Draws a border for the window provided
static void draw_borders(WINDOW *screen) {
    int x;
    int y;
    getmaxyx(screen, y, x);

    // Draw the 4 corners
    mvwprintw(screen, 0, 0, "+");
    mvwprintw(screen, y - 1, 0, "+");
    mvwprintw(screen, 0, x - 1, "+");
    mvwprintw(screen, y - 1, x - 1, "+");

    // Draw sides
    for (int i = 1; i < (y - 1); i++) {
        mvwprintw(screen, i, 0, "|");
        mvwprintw(screen, i, x - 1, "|");
    }

    // Draw top and bottom
    for (int i = 1; i < (x - 1); i++) {
        mvwprintw(screen, 0, i, "-");
        mvwprintw(screen, y - 1, i, "-");
    }
}
