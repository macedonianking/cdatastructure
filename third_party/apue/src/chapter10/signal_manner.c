#include "chapter10/signal_manner.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define LOG_TAG "signal_manner"
#include "thread/thread_looper.h"
#include "utils/log.h"
#include "utils/apue.h"
#include "utils/list.h"

struct signal_message {
    list_head node;
    millis_t time;
    int signo;
};

static sigset_t g_signal_mask;

static DEFINE_LIST_HEAD(g_msg_list_free);
static DEFINE_LIST_HEAD(g_msg_list_used);

static void init_msg_list();
static void proc_msg_list();
static void* signal_manner_signal_thread(void *args);

/**
 * 处理信号的良好方式
 */
void signal_manner_main(int argc, char **argv) {
    pthread_t tid;
    int result;

    init_msg_list();
    result = pthread_create(&tid, NULL, &signal_manner_signal_thread, NULL);
    ALOGE_ALWAYSE_FATAL_IF(result, "pthread_create failure(%s)", strerror(errno));

    pthread_join(tid, NULL);
}

void init_msg_list() {
    struct signal_message *ptr;
    int result;

    // Disable SIGUSR1 for curren thread.
    sigemptyset(&g_signal_mask);
    sigaddset(&g_signal_mask, SIGUSR1);
    result = pthread_sigmask(SIG_BLOCK, &g_signal_mask, NULL);
    ALOGE_ALWAYSE_FATAL_IF(result != 0 , "pthread_sigmask failure(%s)", strerror(errno));

    // Initialize message list.
    for (int i = 0; i < 1000; ++i) {
        ptr = (struct signal_message*) malloc(sizeof(*ptr));
        INIT_LIST_HEAD(&ptr->node);
        ptr->time = 0;
        ptr->signo = 0;
        list_add(&ptr->node, &g_msg_list_free);
    }
}

static void do_proc_signal_message(struct signal_message *ptr) {
    char buf[BUFSIZ];

    get_format_time(ptr->time, buf, BUFSIZ);
    ALOGE("SIGUSR1 happened at %s", buf);
}

void proc_msg_list() {
    struct signal_message *ptr;

    LIST_FOR_EACH_ENTRY_SAFE(ptr, &g_msg_list_used, node) {
        list_del(&ptr->node);
        do_proc_signal_message(ptr);
        ptr->time = 0;
        ptr->signo = 0;
        list_add_tail(&ptr->node, &g_msg_list_free);
    }
}

static void sigaction_sigusr(int signo, siginfo_t *info, void *data) {
    struct signal_message *ptr;

    if (!list_empty(&g_msg_list_free)) {
        ptr = list_entry(g_msg_list_free.next, struct signal_message, node);
        list_del(&ptr->node);
        ptr->time = current_time_millis();
        ptr->signo = signo;
        list_add_tail(&ptr->node, &g_msg_list_used);
    }
}

static void *signal_manner_signal_thread(void *args) {
    sigset_t mask;
    struct sigaction action;
    int result;

    // Test SIGUSR1 block state.
    pthread_sigmask(SIG_BLOCK, NULL, &mask);
    ALOGE_ALWAYSE_FATAL_IF(!sigismember(&mask, SIGUSR1), "signal_manner_signal_thread must  block SIGUSR1");

    // Set sigaction handler for SIGUSR1.
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_flags = SA_SIGINFO;
    action.sa_handler = NULL;
    action.sa_sigaction = &sigaction_sigusr;
    sigemptyset(&action.sa_mask);
    result = sigaction(SIGUSR1, &action, NULL);
    ALOGE_ALWAYSE_FATAL_IF(result, "sigaction FATAL(%s)", strerror(errno));

    // Wait for signal to occure.
    sigemptyset(&mask);
    for (;;) {
        sigsuspend(&mask);
        ALOGE_ALWAYSE_FATAL_IF(errno != EINTR, "sigsuspend FATAL(%s)", strerror(errno));
        proc_msg_list();
    }
    return NULL;
}
