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

/*
 *  Global Variables
 */

static WINDOW *chat_win;
static WINDOW *input_win;
static WINDOW *chat_box;
static WINDOW *input_box;
static int max_y, max_x;    // Console screen dimensions

/*
 *  Function Prototypes
 */

static void refresh_all_win();

/*
 *  Functions
 */

void init_gui() {
    // Set ncurse settings
    initscr();
    noecho();
    curs_set(FALSE);

    getmaxyx(stdscr, max_y, max_x);

    // Initialise windows
    chat_win = newwin(max_y - CHAT_BOX_SIZE, max_x, 0, 0);
    chat_box = newwin(max_y - CHAT_BOX_SIZE - 2, max_x - 2, 1, 1);
    input_win = newwin(CHAT_BOX_SIZE, max_x, max_y - CHAT_BOX_SIZE, 0);
    input_box = newwin(1, max_x - 2, max_y - CHAT_BOX_SIZE + 1, 1);

    // Remove wgetch blocking on the input_box window
    nodelay(input_box, TRUE);

    // Enable scrolling for the chat_box window
    scrollok(chat_box, TRUE);

    // Draw borders
    box(chat_win, 0, 0);
    box(input_win, 0, 0);

    // Print GUI messages on the windows
    mvwprintw(chat_win, 0, 3, "Chat");
    mvwprintw(input_box, 0, 0, "Start typing to chat...");

    // Refresh each window
    refresh_all_win();
}

void resize_gui() {
    int new_y, new_x;

    getmaxyx(stdscr, new_y, new_x);

    if (new_y == max_y && new_x == max_x) {
        // No need to resize
        return;
    }

    max_x = new_x;
    max_y = new_y;

    // Resizing and repositioning
    wresize(chat_win, max_y - CHAT_BOX_SIZE, max_x);
    wresize(chat_box, max_y - CHAT_BOX_SIZE - 2, max_x - 2);
    wresize(input_win, CHAT_BOX_SIZE, max_x);
    wresize(input_box, 1, max_x - 2);
    mvwin(chat_win, 0, 0);
    mvwin(chat_box, 1, 1);
    mvwin(input_win, max_y - CHAT_BOX_SIZE, 0);
    mvwin(input_box, max_y - CHAT_BOX_SIZE + 1, 1);

    // Clear all windows
    wclear(chat_win);
    wclear(chat_box);
    wclear(input_win);
    wclear(input_box);

    // Redraw GUI design
    box(chat_win, 0, 0);
    box(input_win, 0, 0);
    mvwprintw(chat_win, 0, 3, "Chat");
    mvwprintw(input_box, 0, 0, "Start typing to chat...");

    refresh_all_win();
}

int read_input_from_user(char *buff, int *pos, int length) {
    int ch = wgetch(input_box);
    int msg_sent = 0;
    if (ch >= 32 && ch <= 126) {
        // Printable Characters
        if (*pos == 0) {
            wclear(input_box);
            mvwprintw(input_box, 0, 0, "%c", ch);
            buff[*pos] = ch;
            *pos += 1;
        } else if (*pos < length - 2) {
            wprintw(input_box, "%c", ch);
            buff[*pos] = ch;
            *pos += 1;
        }
    } else if (ch == '\n') {
        // Newline

        // Ignore empty messages
        if (*pos != 0) {
            if (*pos < length - 2) {
                buff[*pos] = ch;
                wclear(input_box);
            } else {
                buff[length - 2] = '\n';
                buff[length - 1] = '\0';
                wclear(input_box);
            }
            mvwprintw(input_box, 0, 0, "Start typing to chat...");
            *pos = 0;
            wprintw(chat_box, "%s", buff);
            msg_sent = 1;
        }
    } else if (ch == '\b' || ch == 127) {
        if (*pos > 0) {
            // Backspace
            buff[*pos] = '\0';
            wprintw(input_box, "\b \b");
            *pos -= 1;
        }
    } else if (*pos == 0) {
        // Nothing in the chat box
        mvwprintw(input_box, 0, 0, "Start typing to chat...");
    }

    wrefresh(chat_box);
    return msg_sent;
}

void destroy_gui() {
    delwin(chat_win);
    delwin(input_win);
    delwin(input_box);
    delwin(chat_box);
    endwin();
}

/*
 *  Helper Functions
 */

static void refresh_all_win() {
    wrefresh(chat_win);
    wrefresh(chat_box);
    wrefresh(input_win);
    wrefresh(input_box);
}
