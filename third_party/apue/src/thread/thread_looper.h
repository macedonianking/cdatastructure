#ifndef APUE_THREAD_LOOP_THREAD_LOOPER_H
#define APUE_THREAD_LOOP_THREAD_LOOPER_H

#include <pthread.h>

/**
 * struct _thread_loop wait messages and File descriptor events.
 */
#include "utils/time_util.h"
#include "utils/list.h"

#define MSG_STATE_NONE  0
#define MSG_STATE_USED  1

struct thread_looper;
typedef struct thread_looper looper_t;

struct thread_message;
typedef struct thread_message message_t;

#define THREAD_HANDLER_CLOSED   1

typedef struct thread_handler {
    int state;
    looper_t *looper;
    void (*dispatch_message)(message_t *msg);
    void  (*dispatch_message_dtor)(message_t *msg);
    pthread_mutex_t     lock;
    int                 count;
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

looper_t *get_looper();
int thread_looper_prepare(int options);
int thread_looper_loop_once(looper_t *looper);
void thread_looper_loop();
int thread_looper_quit();

/**
 * Create one thread handler that is associated with the looper target.
 *
 * @param looper
 * @param handler
 */
int open_thread_handler(looper_t *looper, handler_t **handler);
int close_thread_handler(handler_t *handler);

int thread_handler_send_message_at_time(handler_t *handler, nsec_t uptime, message_t *msg);
int thread_handler_send_message_delayed(handler_t *handler, message_t *msg, nsec_t delay);
int thread_handler_send_message(handler_t* handler, message_t *msg);

int acquire_looper(looper_t *looper);
int release_looper(looper_t *looper);

/**
 * thread looper remove all messages.
 */
void thread_handler_remove_messages(handler_t *handler, int what);
message_t *thread_handler_obtain_message(handler_t *handler, int what, int arg1, int arg2, void *data);

/**
 * thread looper remove all messages.
 */
message_t *thread_handler_obtain_message(handler_t *handler, int what, int arg1, int arg2, void *data);
/**
 * thread looper remove all messages.
 */
message_t *thread_handler_obtain_message(handler_t *handler, int what, int arg1, int arg2, void *data);

#endif // APUE_THREAD_LOOP_THREAD_LOOPER_H
