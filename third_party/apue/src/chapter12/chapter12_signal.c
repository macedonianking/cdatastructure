#include "chapter12/chapter12.h"

#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

#include "utils/apue.h"
#include "utils/list.h"
#include "utils/time_util.h"

void chapter12_8(int argc, char **argv) {
    chapter12_8_1(argc, argv);
}

struct chapter12_8_1_message {
    list_head   m_node;
    long        m_time;
    void (*m_handler)(void*);
    void *m_data;
};

struct chapter12_8_1_params {
    pthread_mutex_t     p_lock;
    pthread_cond_t      p_cond;
    int                 p_is_finish;
    list_head           p_wait_list;
    list_head           p_done_list;
    sigset_t            p_sig_mask;
};

static void chapter12_8_1_post_timer(struct chapter12_8_1_params *params, long millis, void (*handler)(void*), void *data) {
    struct chapter12_8_1_message *msg, *ptr;
    list_head *next;
    struct itimerval val;

    msg = (struct chapter12_8_1_message*) malloc(sizeof(*msg));
    INIT_LIST_HEAD(&msg->m_node);
    msg->m_time = millis + current_time_millis();
    msg->m_handler = handler;
    msg->m_data = data;

    millis = 0;
    if (pthread_mutex_lock(&params->p_lock)) {
        LOGE("pthread_mutex_lock FATAL");
        exit(-1);
    }
    next = &params->p_wait_list;
    LIST_FOR_EACH_ENTRY(ptr, &params->p_wait_list, m_node) {
        if (msg->m_time <= ptr->m_time) {
            next = &ptr->m_node;
        }
    }
    list_add_tail(&msg->m_node, next);
    if (params->p_wait_list.next == &msg->m_node) {
        millis = msg->m_time - current_time_millis();
        if (millis < 1) {
            millis = 1;
        }
        memset(&val, 0, sizeof(val));
        millis_to_timeval(millis, &val.it_value);
        if (setitimer(ITIMER_REAL, &val, NULL)) {
            LOGE("setitimer FATAL");
            exit(-1);
        }
    }
    pthread_mutex_unlock(&params->p_lock);
}

static void chapter12_8_1_check_expired(struct chapter12_8_1_params *params) {

}

static void* chapter12_8_1_thread_func(void *args) {
    sigset_t mask;
    int signo;

    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);

    for (;;) {
        if (sigwait(&mask, &signo)) {
            if (errno == EINTR) {
                continue;
            } else {
                LOGE("sigwait FATAL");
                exit(-1);
            }
        } else {

        }
    }
    return NULL;
}

static void chapter12_8_1_handler(void *args) {
}

static void chapter12_8_1_check_done_list_locked(struct chapter12_8_1_params *params) {
}

void chapter12_8_1(int argc, char **argv) {
    struct chapter12_8_1_params params;
    pthread_t tid;
    sigset_t old_mask;

    pthread_mutex_init(&params.p_lock, NULL);
    pthread_cond_init(&params.p_cond, NULL);
    INIT_LIST_HEAD(&params.p_wait_list);
    INIT_LIST_HEAD(&params.p_done_list);
    params.p_is_finish = 0;

    sigemptyset(&params.p_sig_mask);
    sigaddset(&params.p_sig_mask, SIGALRM);
    if (pthread_sigmask(SIG_BLOCK, &params.p_sig_mask, &old_mask)) {
        LOGE("pthread_sigmask FATAL");
        exit(-1);
    }

    chapter12_8_1_post_timer(&params, SECOND_IN_MILLIS, &chapter12_8_1_handler, NULL);
    chapter12_8_1_post_timer(&params, 3 * SECOND_IN_MILLIS, &chapter12_8_1_handler, NULL);

    if (pthread_create(&tid, NULL, &chapter12_8_1_thread_func, &params)) {
        LOGE("pthread_create FATAL");
        exit(-1);
    }

    pthread_mutex_lock(&params.p_lock);
    for (;;) {
        chapter12_8_1_check_done_list_locked(&params);
        if (params.p_is_finish) {
            break;
        }
        if (pthread_cond_wait(&params.p_cond, &params.p_lock)) {
            LOGE("pthread_cond_wait FATAL");
            break;
        }
    }
    pthread_mutex_unlock(&params.p_lock);

    pthread_cond_destroy(&params.p_cond);
    pthread_mutex_destroy(&params.p_lock);
    if (pthread_sigmask(SIG_SETMASK, &old_mask, NULL)) {
        LOGE("pthread_sigmask FATAL");
        exit(-1);
    }
}
