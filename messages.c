/*
 *  Library Includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
 *  Non-library Includes
 */

#include "messages.h"
#include "gui.h"

/*
 *  Global Variables
 */

static struct message *msgs[N_MAX_MSGS];   // Circular array buffer
static int msg_start = 0;
static int msg_end = 0;
static int n_msgs = 0;
static pthread_mutex_t msg_mutex;

void init_msg_mutex() {
    int result = pthread_mutex_init(&msg_mutex, NULL);
    if (result) {
        fprintf(stderr, "pthread_mutex_init: failed with error %d\n", result);
        exit(EXIT_FAILURE);
    }
}

struct message *new_message(char *msg, char *sender) {
    struct message *new = malloc(sizeof(struct message));
    if (new == NULL) {
        return NULL;
    }

    sprintf(new->content, "%s", msg);
    if (sender != NULL) {
        sprintf(new->sender, "%s", sender);
    }

    return new;
}

void add_to_messages(struct message *msg) {
    add_msg_to_chat_box(msg);

    pthread_mutex_lock(&msg_mutex);
    if (n_msgs == 0) {
        msgs[msg_start] = msg;
        msg_end++;
    } else {
        if (msg_start == msg_end) {
            destroy_message(msgs[msg_start]);
            msgs[msg_end] = msg;
            msg_start++;
            if (msg_start == N_MAX_MSGS) {
                msg_start = 0;
            }
        } else {
            if (msgs[msg_end]) {
                destroy_message(msgs[msg_end]);
            }
            msgs[msg_end] = msg;
        }
        msg_end++;
        if (msg_end == N_MAX_MSGS) {
            msg_end = 0;
        }
    }

    if (n_msgs < N_MAX_MSGS) {
        n_msgs++;
    }

    pthread_mutex_unlock(&msg_mutex);
}

void print_all_messages() {
    pthread_mutex_lock(&msg_mutex);
    int i = msg_start;
    int count = 0;
    while (count < n_msgs) {
        add_msg_to_chat_box(msgs[i]);
        count++;
        i++;
        if (i == N_MAX_MSGS) {
            i = 0;
        }
    }
    pthread_mutex_unlock(&msg_mutex);
}

void destroy_message(struct message *msg) {
    free(msg);
}
