#include "chapter100/chapter100.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define LOG_TAG "chapter100"
#include "thread/thread_looper.h"
#include "utils/log.h"
#include "utils/apue.h"
#include "utils/list.h"

struct signal_message {
    list_head node;
    millis_t time;
};

static sigset_t g_sig_mask;
static sigset_t g_sig_save;
static int g_disable_count = 0;
static DEFINE_LIST_HEAD(g_msg_list_free);
static DEFINE_LIST_HEAD(g_msg_list_used);

static void init_msg_list();
static void proc_msg_list();

static inline void disable_signal() {
    if (g_disable_count++ == 0) {
        if (sigprocmask(SIG_BLOCK, &g_sig_mask, &g_sig_save)) {
            ALOGE("disable_signal failure(%s)", strerror(errno));
        }
    }
}

static inline void enable_signal() {
    if (--g_disable_count == 0) {
        if (sigprocmask(SIG_SETMASK, &g_sig_save, NULL)) {
            ALOGE("enable_signal failure(%s)", strerror(errno));
        } else {
            proc_msg_list();
        }
    }
}

void init_msg_list() {
    struct signal_message *ptr;

    disable_signal();
    for (int i = 0; i < 1000; ++i) {
        ptr = (struct signal_message*) malloc(sizeof(*ptr));
        INIT_LIST_HEAD(&ptr->node);
        ptr->time = 0;
        list_add(&ptr->node, &g_msg_list_free);
    }
    enable_signal();
}

static void do_proc_sig_message(struct signal_message *ptr) {
    char buf[BUFSIZ];

    get_format_time(ptr->time, buf, BUFSIZ);
    ALOGE("SIGUSR1 happened at %s\n", buf);
}

void proc_msg_list() {
    sigset_t mask, save;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    struct signal_message *ptr;

    if (!sigprocmask(SIG_BLOCK, &mask, &save)) {
        LIST_FOR_EACH_ENTRY_SAFE(ptr, &g_msg_list_used, node) {
            list_del(&ptr->node);
            do_proc_sig_message(ptr);
            ptr->time = 0;
            list_add_tail(&ptr->node, &g_msg_list_free);
        }
        sigprocmask(SIG_SETMASK, &save, NULL);
    }
}

void sigaction_sigusr(int signo, siginfo_t *info, void *data) {
    ALOGE("sigaction_sigusr called");
    struct signal_message *ptr;

    if (!list_empty(&g_msg_list_free)) {
        ptr = list_entry(g_msg_list_free.next, struct signal_message, node);
        list_del(&ptr->node);
        ptr->time = current_time_millis();
        list_add_tail(&ptr->node, &g_msg_list_used);
    }
}

int chapter100_main(int argc, char **argv) {
    struct sigaction action;
    sigset_t suspend_mask;

    init_msg_list();

    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_SIGINFO;
    action.sa_handler = NULL;
    action.sa_sigaction = &sigaction_sigusr;
    if (sigaction(SIGUSR1, &action, NULL)) {
        ALOGE("sigaction FATAL(%s)", strerror(errno));
        exit(-1);
    }

    sigemptyset(&suspend_mask);
    for (;;) {
        disable_signal();
        sleep(1);
        sleep(1);
        enable_signal();
    }

    return 0;
}
