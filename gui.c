/*
 *  Library Includes
 */

#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

/*
 *  Non-library Includes
 */

#include "gui.h"
#include "messages.h"

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
static void resize_gui(int sig);

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

    // Enable scrolling for chat_box and input_box windows
    scrollok(chat_box, TRUE);
    scrollok(input_box, TRUE);

    // Draw borders
    box(chat_win, 0, 0);
    box(input_win, 0, 0);

    // Print GUI messages on the windows
    mvwprintw(chat_win, 0, 3, "Chat");
    mvwprintw(input_box, 0, 0, "Start typing to chat...");

    // Refresh each window
    refresh_all_win();

    // Resize gui when console is resized
    signal(SIGWINCH, resize_gui);
}

int read_input_from_user(char *buff, int *pos, int length) {
    int ch = getch();
    int send_msg = 0;
    if (ch >= 32 && ch <= 126) {
        // Printable Characters
        if (*pos == 0) {
            wclear(input_box);
            mvwprintw(input_box, 0, 0, "%c", ch);
            buff[*pos] = ch;
            *pos += 1;
            curs_set(TRUE);
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
            send_msg = 1;
            curs_set(FALSE);
        }
    } else if (ch == '\b' || ch == 127) {
        if (*pos > 0) {
            // Backspace
            buff[*pos] = '\0';
            wprintw(input_box, "\b \b");
            *pos -= 1;
        }

        if (*pos == 0) {
            mvwprintw(input_box, 0, 0, "Start typing to chat...");
            curs_set(FALSE);
        }
    } else if (*pos == 0) {
        // Nothing in the chat box
        mvwprintw(input_box, 0, 0, "Start typing to chat...");
    }

    wrefresh(input_box);
    return send_msg;
}

void add_msg_to_chat_box(struct message *msg) {
    if (strlen(msg->sender) == 0) {
        wprintw(chat_box, "[SERVER]: %s", msg->content);
    } else {
        wprintw(chat_box, "[%s]: %s", msg->sender, msg->content);
    }
    refresh_all_win();
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
    wrefresh(stdscr);
    wrefresh(chat_win);
    wrefresh(chat_box);
    wrefresh(input_win);
    wrefresh(input_box);
}

// Checks if gui needs to be resized and resizes if needed
static void resize_gui(int sig) {
    endwin();
    refresh_all_win();

    getmaxyx(stdscr, max_y, max_x);

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

    // Redraw GUI design
    box(chat_win, 0, 0);
    box(input_win, 0, 0);
    mvwprintw(chat_win, 0, 3, "Chat");

    refresh_all_win();

    // Redraw chat messages
    print_all_messages();
}
