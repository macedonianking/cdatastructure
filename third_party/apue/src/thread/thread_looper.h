#ifndef APUE_THREAD_LOOP_THREAD_LOOPER_H
#define APUE_THREAD_LOOP_THREAD_LOOPER_H

/**
 * struct _thread_loop wait messages and File descriptor events.
 */
#include "utils/time_util.h"
#include "utils/list.h"

struct _thread_looper;
typedef struct _thread_looper looper_t;

struct _thread_message;

typedef struct _thread_handler {
    void (*dispatch_message)(struct _thread_message *msg);
} handler_t;

typedef struct _thread_message {
    list_head   node;
    int         state;
    handler_t   *target;
    nsec_t      uptime;
    void        (*callback)(struct _thread_message*);
    void        (*cancel_callback)(struct _thread_message*);
    int         what;
    int         arg1;
    int         arg2;
    void        *data;
} message_t;

looper_t            *thread_looper_create();
void                thread_looper_free(looper_t*);

looper_t *get_looper();
int thread_looper_prepare(int options);
int thread_looper_loop_once(looper_t *looper);
int thread_looper_send_message_at_time(looper_t *looper, handler_t *handler, nsec_t uptime, message_t *msg);
int thread_looper_send_message_dealyed(looper_t *looper, handler_t *handler, message_t *msg, nsec_t delay);
int thread_looper_send_message(looper_t *looper, handler_t* handler, message_t *msg);

#endif // APUE_THREAD_LOOP_THREAD_LOOPER_H
