#ifndef APUE_THREAD_LOOP_THREAD_LOOPER_H
#define APUE_THREAD_LOOP_THREAD_LOOPER_H

/**
 * struct _thread_loop wait messages and File descriptor events.
 */
#include "utils/time_util.h"
#include "utils/list.h"

#define MSG_STATE_NONE  0
#define MSG_STATE_USED  1

struct _thread_looper;
typedef struct _thread_looper looper_t;

struct thread_message;
typedef struct thread_message message_t;

typedef struct thread_handler {
    looper_t *looper;
    void (*dispatch_message)(message_t *msg);
    int  (*dispatch_message_dtor)(message_t *msg);
} handler_t;

struct thread_message {
    list_head   node;
    int         state;
    handler_t   *target;
    nsec_t      uptime;
    void        (*callback)(message_t*);
    int         what;
    int         arg1;
    int         arg2;
    void        *data;
    void        (*data_dtor)(message_t *t);
};

looper_t            *thread_looper_create();
void                thread_looper_free(looper_t*);

looper_t *get_looper();
int thread_looper_prepare(int options);
int thread_looper_loop_once(looper_t *looper);

int thread_looper_send_message_at_time(looper_t *looper, handler_t *handler, nsec_t uptime, message_t *msg);
int thread_looper_send_message_dealyed(looper_t *looper, handler_t *handler, message_t *msg, nsec_t delay);
int thread_looper_send_message(looper_t *looper, handler_t* handler, message_t *msg);

/**
 * thread looper remove all messages.
 */
void thread_looper_remove_messages(looper_t *looper, handler_t *handler, int what);

message_t *thread_handler_obtain_message(handler_t *handler, int what, int arg1, int arg2, void *data);

#endif // APUE_THREAD_LOOP_THREAD_LOOPER_H
