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

#define MSG_STATE_NONE      0
#define MSG_STATE_USING     1

struct _thread_looper {
    int         tl_rdfd;
    int         tl_wrfd;
    int         tl_epollfd;
    list_head   tl_list;
    pthread_mutex_t     tl_lock;
};

static pthread_key_t g_thread_loop_key;
static pthread_once_t g_thread_loop_init_once = PTHREAD_ONCE_INIT;

static void wakeup_looper(looper_t *looper);

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

    ptr = (looper_t*) malloc(sizeof(struct _thread_looper));
    memset(ptr, 0, sizeof(*ptr));
    INIT_LIST_HEAD(&ptr->tl_list);
    ptr->tl_epollfd = epollfd;
    ptr->tl_rdfd = rdfd;
    ptr->tl_wrfd = wrfd;

    r = pthread_mutex_init(&ptr->tl_lock, NULL);
    ALOGE_IF(r, "thread_looper_create. pthread_mutex_init failure(%s)", strerror(errno));
    goto done;

meet_error:
    if (rdfd != -1)
        close(rdfd);
    if (wrfd != -1)
        close(wrfd);
    if (epollfd != -1)
        close(epollfd);

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
            r = 0;
        } else if (looper) {
            thread_looper_free(looper);
            looper = NULL;
        }
    } else {
        ALOGW("thread_looper_prepare. The thread#%d already has a looper.", sys_gettid());
    }

    return r;
}

int thread_looper_send_message_dealyed(looper_t *looper,
    handler_t *handler, message_t *msg, nsec_t delay) {

    return thread_looper_send_message_at_time(looper,
        handler, system_clock_nano_uptime() + delay, msg);
}

int thread_looper_send_message_at_time(looper_t *looper,
    handler_t *handler, nsec_t uptime, message_t *msg) {

    message_t *next, *item;
    int result, is_first;

    result = pthread_mutex_lock(&looper->tl_lock);
    ALOGE_IF(result, "thread_looper_send_message_at_time.lock failure(%s)", strerror(errno));

    next = NULL;
    LIST_FOR_EACH_ENTRY(item, &looper->tl_list, node) {
        if (uptime < item->uptime) {
            next = item;
            break;
        }
    }
    msg->uptime = uptime;
    if (next != NULL) {
        list_add_tail(&msg->node, &next->node);
    } else {
        list_add_tail(&msg->node, &looper->tl_list);
    }
    is_first = msg->node.prev == &looper->tl_list;

    result = pthread_mutex_unlock(&looper->tl_lock);
    ALOGE_IF(result, "thread_looper_send_message_at_time.unlock failure(%s)", strerror(errno));

    if (is_first) {
        wakeup_looper(looper);
    }
    return 0;
}

int thread_looper_send_message(looper_t *looper, handler_t *handler,
    message_t *msg) {
    return thread_looper_send_message_at_time(looper, handler,
        system_clock_nano_uptime(), msg);
}

void wakeup_looper(looper_t *looper) {
    int n;

    while((n = write(looper->tl_wrfd, "w", 1)) != -1 || errno == EINTR) {
        ;
    }
    LOG_ALWAYSE_FATAL_IF(n != 1, "wakeup_looper failure(%s)", strerror(errno));
}
