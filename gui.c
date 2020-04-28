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
static int max_y, max_x;                    // Console screen dimensions
static char input_buff[MSG_MAX + 1] = {0};  // Input buffer text

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
    keypad(stdscr, TRUE);
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

char *read_input_from_user() {
    int ch;
    int i = 0;
    bzero(input_buff, MSG_MAX);
    while ((ch = getch()) != '\n' || i == 0) {
        if (ch >= 32 && ch <= 126 && i < MSG_MAX + 1) {
            wclear(input_box);
            if (i == 0) {
                curs_set(TRUE);
                mvwprintw(input_box, 0, 0, "%c", ch);
            } else {
                mvwprintw(input_box, 0, 0, "%s", input_buff);
                wprintw(input_box, "%c", ch);
            }
            input_buff[i] = ch;
            i++;
        } else if (ch == KEY_BACKSPACE && i > 0) {
            i--;
            input_buff[i] = '\0';
            wclear(input_box);

            if (i == 0) {
                mvwprintw(input_box, 0, 0, "Start typing to chat...");
                curs_set(FALSE);
            } else {
                mvwprintw(input_box, 0, 0, "%s", input_buff);
            }
        }
        wrefresh(input_box);
    }

    wclear(input_box);
    mvwprintw(input_box, 0, 0, "Start typing to chat...");
    curs_set(FALSE);

    return input_buff;
}

void add_msg_to_chat_box(struct message *msg) {
    if (strlen(msg->sender) == 0) {
        wprintw(chat_box, "[SERVER]: %s\n", msg->content);
    } else {
        wprintw(chat_box, "[%s]: %s\n", msg->sender, msg->content);
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
    wclear(input_box);

    // Reprint input_box message
    if (strlen(input_buff) == 0) {
        mvwprintw(input_box, 0, 0, "Start typing to chat...");
    } else {
        mvwprintw(input_box, 0, 0, "%s", input_buff);
    }

    // Redraw GUI design
    box(chat_win, 0, 0);
    box(input_win, 0, 0);
    mvwprintw(chat_win, 0, 3, "Chat");

    refresh_all_win();

    // Redraw chat messages
    print_all_messages();
}
