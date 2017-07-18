#include "thread/thread_looper.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include "thread/thread_loop.h"

#define LOG_TAG "thread_looper"
#include "utils/log.h"

#define THREAD_FLAG_QUIT        1
#define THREAD_FLAG_ATTACHED    (1 << 1)

struct thread_looper {
    int                 tl_count;
    int                 tl_state;
    int                 tl_rdfd;
    int                 tl_wrfd;
    int                 tl_epollfd;
    list_head           tl_list;
    pthread_mutex_t     tl_lock;
    pid_t               tl_tid;
};

static pthread_key_t g_thread_loop_key;
static pthread_once_t g_thread_loop_init_once = PTHREAD_ONCE_INIT;

static void wakeup_looper(looper_t *looper);
static message_t *obtain_message();
static void send_back_message(message_t *msg);

static void dispatch_message_dtor(message_t *msg);
/**
 * Call the destrcutor for the message and release the message object.
 */
static void dispatch_one_message_dtor_and_release(message_t *msg);
static void dispatch_message_dtor_and_release(list_head *list);

static looper_t *thread_looper_create();
static void thread_looper_free(looper_t*);
static int acquire_handler(handler_t *handler);
static int release_handler(handler_t *handler);

static void free_handler(handler_t *handler);

static void default_handler_dispatch_message(message_t *msg);
static void default_handler_dispatch_message_dtor(message_t *msg);

static int set_pipe_non_block(int fd) {
    int flags;
    int r;

    r = -1;
    if ((flags = fcntl(fd, F_GETFL)) != -1
        && !fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
        r = 0;
    }
    return r;
}

looper_t *thread_looper_create() {
    int pipefd[2];
    looper_t *ptr;
    int rdfd, wrfd, epollfd;
    struct epoll_event event;
    int r;

    ptr = NULL;
    rdfd = wrfd = epollfd = -1;

    if (pipe(pipefd) == -1) {
        ALOGE("thread_looper_create. create pipe failure(%s)", strerror(errno));
        goto meet_error;
    }
    rdfd = pipefd[0];
    wrfd = pipefd[1];

    if (set_pipe_non_block(rdfd) || set_pipe_non_block(wrfd)) {
        ALOGE("thread_looper_create fcntl append O_NONBLOCK failure");
    }

    if ((epollfd = epoll_create(1)) == -1) {
        ALOGE("thread_looper_create. epoll_create failure(%s)", strerror(errno));
        goto meet_error;
    }

    memset(&event, 0, sizeof(struct epoll_event));
    event.events = EPOLLIN;
    event.data.fd = rdfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, rdfd, &event)) {
        ALOGE("thread_looper_create. epoll_ctl failure(%s)", strerror(errno));
        goto meet_error;
    }

    ptr = (looper_t*) malloc(sizeof(struct thread_looper));
    memset(ptr, 0, sizeof(*ptr));
    INIT_LIST_HEAD(&ptr->tl_list);
    ptr->tl_epollfd = epollfd;
    ptr->tl_rdfd = rdfd;
    ptr->tl_wrfd = wrfd;
    ptr->tl_count = 1;
    ptr->tl_tid = -1;

    r = pthread_mutex_init(&ptr->tl_lock, NULL);
    ALOGE_IF(r, "thread_looper_create. pthread_mutex_init failure(%s)", strerror(errno));
    goto done;

meet_error:
    if (rdfd != -1) {
        close(rdfd);
    }
    if (wrfd != -1) {
        close(wrfd);
    }
    if (epollfd != -1) {
        close(epollfd);
    }
done:
    return ptr;
}

void thread_looper_free(looper_t* ptr) {
    close(ptr->tl_rdfd);
    close(ptr->tl_wrfd);
    close(ptr->tl_epollfd);
    free(ptr);
}

static void tls_key_dctor(void *args) {
    thread_looper_free((looper_t*) args);
}

static void tls_key_init() {
    int r = pthread_key_create(&g_thread_loop_key, &tls_key_dctor);
    ALOGE_IF(r, "pthread_key_create failure(%s)", strerror(errno));
}

looper_t *get_looper() {
    pthread_once(&g_thread_loop_init_once, &tls_key_init);
    return (looper_t*) pthread_getspecific(g_thread_loop_key);
}

int thread_looper_prepare(int options) {
    looper_t *looper;
    int r;

    r = -1;
    if (!(looper = get_looper())) {
        looper = thread_looper_create();
        if (looper && !pthread_setspecific(g_thread_loop_key, looper)) {
            looper->tl_tid = sys_gettid();
            looper->tl_state |= THREAD_FLAG_ATTACHED;
            r = 0;
        } else if (looper) {
            release_looper(looper);
            looper = NULL;
        }
    } else {
        ALOGW("thread_looper_prepare. The thread#%d already has a looper.", sys_gettid());
    }

    return r;
}

int thread_handler_send_message_delayed(handler_t *handler, message_t *msg, nsec_t delay) {
    return thread_handler_send_message_at_time(handler, system_clock_nano_uptime() + delay, msg);
}

int thread_handler_send_message_at_time(handler_t *handler, nsec_t uptime, message_t *msg) {
    message_t *next, *item;
    int result, is_first;
    int r;
    looper_t *looper;

    is_first = 0;
    r = 0;
    looper = handler->looper;
    DCHECK(msg->state == MSG_STATE_NONE);
    result = pthread_mutex_lock(&looper->tl_lock);
    ALOGE_IF(result, "thread_handler_send_message_at_time.lock failure(%s)", strerror(errno));
    if (looper->tl_state & THREAD_FLAG_QUIT) {
        r = -1;
        goto done;
    }

    next = NULL;
    LIST_FOR_EACH_ENTRY(item, &looper->tl_list, node) {
        if (uptime < item->uptime) {
            next = item;
            break;
        }
    }
    msg->uptime = uptime;
    msg->state = MSG_STATE_USED;
    if (next != NULL) {
        list_add_tail(&msg->node, &next->node);
    } else {
        list_add_tail(&msg->node, &looper->tl_list);
    }
    is_first = msg->node.prev == &looper->tl_list;

done:
    result = pthread_mutex_unlock(&looper->tl_lock);
    ALOGE_IF(result, "thread_handler_send_message_at_time.unlock failure(%s)", strerror(errno));

    if (is_first) {
        wakeup_looper(looper);
    }
    if (r) {
        dispatch_one_message_dtor_and_release(msg);
    }
    return r;
}

int thread_handler_send_message(handler_t *handler, message_t *msg) {
    return thread_handler_send_message_at_time(handler, system_clock_nano_uptime(), msg);
}

void wakeup_looper(looper_t *looper) {
    int n;

    while((n = write(looper->tl_wrfd, "w", 1)) == -1 && errno == EINTR) {
        ;
    }
    ALOGE_IF(n != 1, "wakeup_looper failure(%s)", strerror(errno));
}

void thread_handler_remove_messages(handler_t *handler, int what) {
    message_t *msg;
    looper_t *looper;
    DEFINE_LIST_HEAD(list);

    looper = handler->looper;
    pthread_mutex_lock(&looper->tl_lock);
    LIST_FOR_EACH_ENTRY_SAFE(msg, &looper->tl_list, node) {
        if (msg->target == handler) {
            list_del(&msg->node);
            list_add_tail(&msg->node, &list);
        }
    }
    pthread_mutex_unlock(&looper->tl_lock);
    dispatch_message_dtor_and_release(&list);
}

void thread_handler_clear_messages(handler_t *handler) {
    message_t *msg;
    looper_t *looper;
    DEFINE_LIST_HEAD(list);

    looper = handler->looper;
    pthread_mutex_lock(&looper->tl_lock);
    LIST_FOR_EACH_ENTRY_SAFE(msg, &looper->tl_list, node) {
        if (msg->target == handler) {
            list_del(&msg->node);
            list_add_tail(&msg->node, &list);
        }
    }
    pthread_mutex_unlock(&looper->tl_lock);
    dispatch_message_dtor_and_release(&list);
}

static void thread_looper_clear_messages(looper_t *looper) {
    message_t *msg;
    DEFINE_LIST_HEAD(list);

    pthread_mutex_lock(&looper->tl_lock);
    LIST_FOR_EACH_ENTRY_SAFE(msg, &looper->tl_list, node) {
            list_del(&msg->node);
            list_add_tail(&msg->node, &list);
    }
    pthread_mutex_unlock(&looper->tl_lock);
    dispatch_message_dtor_and_release(&list);
}

message_t *obtain_message() {
    message_t *ptr;

    ptr = (message_t*) malloc(sizeof(*ptr));
    ALOGE_ALWAYSE_FATAL_IF(!ptr, "obtain_message FATAL");
    memset(ptr, 0, sizeof(*ptr));
    return ptr;
}

void send_back_message(message_t *ptr) {
    DCHECK(ptr && ptr->state == MSG_STATE_NONE);
    memset(ptr, 0, sizeof(*ptr));
    free(ptr);
}

void dispatch_message_dtor(message_t *msg) {
    if (msg->data) {
        if (msg->data_dtor) {
            msg->data_dtor(msg);
        } else {
            msg->target->dispatch_message_dtor(msg);
        }
    }
    release_handler(msg->target);
    memset(msg, 0, sizeof(*msg));
    msg->state = MSG_STATE_NONE;
}

void dispatch_message_dtor_and_release(list_head *list) {
    message_t *msg;

    // Release all removed messages.
    LIST_FOR_EACH_ENTRY_SAFE(msg, list, node) {
        list_del(&msg->node);
        dispatch_message_dtor(msg);
        send_back_message(msg);
    }
}

void dispatch_one_message_dtor_and_release(message_t *msg) {
    dispatch_message_dtor(msg);
    send_back_message(msg);
}

message_t *thread_handler_obtain_message(handler_t *handler, int what, int arg1, int arg2, void *data) {
    message_t *msg;

    msg = NULL;
    if (!acquire_handler(handler)) {
        msg = obtain_message();
        msg->state = MSG_STATE_NONE;
        msg->what= what;
        msg->arg1 = arg1;
        msg->arg2 = arg2;
        msg->data = data;
        msg->target = handler;
    }
    return msg;
}

int thread_looper_quit() {
    int r;

    r = -1;
    looper_t* item = get_looper();
    if (item != NULL && !pthread_setspecific(g_thread_loop_key, NULL)) {
        r = 0;
        pthread_mutex_lock(&item->tl_lock);
        close(item->tl_rdfd);
        close(item->tl_wrfd);
        item->tl_rdfd = item->tl_wrfd = -1;
        item->tl_state &= THREAD_FLAG_ATTACHED;
        item->tl_state |= THREAD_FLAG_QUIT;
        pthread_mutex_unlock(&item->tl_lock);
        thread_looper_clear_messages(item);
        release_looper(item);
    }
    return r;
}

int acquire_looper(looper_t *looper) {
    DCHECK(looper != NULL);
    int r;

    pthread_mutex_lock(&looper->tl_lock);
    if (++looper->tl_count > 0) {
        r = 0;
    } else {
        --looper->tl_count;
        r = -1;
    }
    pthread_mutex_unlock(&looper->tl_lock);

    return r;
}

int release_looper(looper_t *looper) {
    DCHECK(looper != NULL);
    int r, free;

    r = 0;
    free = 0;
    pthread_mutex_lock(&looper->tl_lock);
    if (!--looper->tl_count) {
        free = 1;
    }
    pthread_mutex_unlock(&looper->tl_lock);
    if (free) {
        thread_looper_free(looper);
    }
    return r;
}

int acquire_handler(handler_t *handler) {
    int r;

    pthread_mutex_lock(&handler->lock);
    if (++handler->count > 0) {
        r = 0;
    } else {
        --handler->count;
        r = -1;
    }
    pthread_mutex_unlock(&handler->lock);
    return r;
}

int release_handler(handler_t *handler) {
    int free;

    free = 0;
    pthread_mutex_lock(&handler->lock);
    if (!--handler->count) {
        free = 1;
    }
    pthread_mutex_unlock(&handler->lock);
    if (free) {
        free_handler(handler);
    }
    return 0;
}

/**
 * Free the memory area for the handler, release the reference to the looper.
 */
void free_handler(handler_t *handler) {
    if (handler->looper) {
        release_looper(handler->looper);
        handler->looper = NULL;
    }
    pthread_mutex_destroy(&handler->lock);
    handler->dispatch_message = NULL;
    handler->dispatch_message_dtor = NULL;
    free(handler);
}

int open_thread_handler(looper_t *looper, handler_t **out) {
    handler_t *ptr;
    int r;

    r = -1;
    if (!looper) {
        looper = get_looper();
    }
    if (looper && out) {
        if (!acquire_looper(looper)) {
            ptr = (handler_t*) malloc(sizeof(*ptr));
            if (!ptr) {
                release_looper(looper);
            } else {
                ptr->looper = looper;
                pthread_mutex_init(&ptr->lock, NULL);
                ptr->count = 1;
                ptr->dispatch_message = &default_handler_dispatch_message;
                ptr->dispatch_message_dtor = &default_handler_dispatch_message_dtor;
                *out = ptr;
                r = 0;
            }
        }
    }

    return r;
}

int close_thread_handler(handler_t *handler) {
    int r;

    pthread_mutex_lock(&handler->lock);
    if (!(handler->state & THREAD_HANDLER_CLOSED)) {
        handler->state |= THREAD_HANDLER_CLOSED;
        pthread_mutex_unlock(&handler->lock);
        r = 0;
    } else {
        pthread_mutex_unlock(&handler->lock);
        r = 1;
    }
    return r;
}

void default_handler_dispatch_message(message_t *msg) {
    ALOGE("default_handler_dispatch_message for message->(%d, %d, %d, %p)",
        msg->what, msg->arg1, msg->arg2, msg->data);
}

void default_handler_dispatch_message_dtor(message_t *msg) {
    if (msg->data) {
        ALOGE("default_handler_dispatch_message_dtor for messages(%d, %d, %d, %p)",
            msg->what, msg->arg1, msg->arg2, msg->data);
    }
}

static void consume_all_pipe_data(int fd) {
    int buf[128];
    int n;

    for (;;) {
        if ((n = read(fd, buf, 128)) == -1) {
            if (errno != EINTR) {
                break;
            }
        } else if (n == 0) {
            break;
        }
    }
}

static int thread_looper_next_message(looper_t *looper, message_t **msg) {
    struct epoll_event events;
    millis_t wait_time;
    nsec_t now;
    message_t *ptr;
    int r;
    int result;

    wait_time = 0;
start:
    if (wait_time) {
        memset(&events, 0, sizeof(events));
        result = epoll_wait(looper->tl_epollfd, &events, 1, wait_time);
        if (result > 0 && events.data.fd == looper->tl_rdfd && events.events & (EPOLLIN | EPOLLPRI)) {
            consume_all_pipe_data(looper->tl_rdfd);
        }
        if (result == -1 && errno != EINTR) {
            ALOGE("thread_looper_next_message epoll_wait failure->looper=%p error=%s", looper, strerror(errno));
        }
    }

    pthread_mutex_lock(&looper->tl_lock);
    if (looper->tl_state & THREAD_FLAG_QUIT) {
        r = -1;
        goto out;
    }
    if (list_empty(&looper->tl_list)) {
        wait_time = DAY_IN_MILLIS;
        goto unlock_and_wait;
    }

    ptr = list_entry(looper->tl_list.next, message_t, node);
    now = system_clock_nano_uptime();
    if (ptr->uptime <= now) {
        r = 0;
        list_del(&ptr->node);
        *msg = ptr;
        goto out;
    }
    wait_time = (ptr->uptime - now) / MILLIS_IN_NANOS;

unlock_and_wait:
    pthread_mutex_unlock(&looper->tl_lock);
    goto start;

out:
    pthread_mutex_unlock(&looper->tl_lock);
    return r;
}

static void dispatch_message(message_t *msg) {
    if (msg->callback) {
        msg->callback(msg);
    } else {
        msg->target->dispatch_message(msg);
    }
    release_handler(msg->target);
    memset(msg, 0, sizeof(*msg));
    msg->state = MSG_STATE_NONE;
    send_back_message(msg);
}

void thread_looper_loop() {
    looper_t *looper = get_looper();
    message_t *msg;
    if (!looper) {
        return;
    }

    acquire_looper(looper);
    for (;;) {
        if (thread_looper_next_message(looper, &msg)) {
            break;
        }
        dispatch_message(msg);
    }
    release_looper(looper);
}
