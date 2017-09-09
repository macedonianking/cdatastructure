#include "chapter10/chapter10.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define LOG_TAG "chapter10"
#include "utils/apue.h"
#include "utils/log.h"
#include "utils/math_help.h"
#include "utils/string_util.h"


int chapter10_main(int argc, char **argv) {
    chapter10_main_1(argc, argv);
    return 0;
}

void my_signal_handler(int signo, siginfo_t *info, void *context);
static void *start_routing(void *);

void chapter10_main_1(int argc, char **argv) {
    sigset_t mask;
    pthread_t tid;
    int r;
    struct timespec req;

    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    r = pthread_create(&tid, NULL, &start_routing, NULL);
    ALOGE_ALWAYSE_FATAL_IF(r, "pthread_create FATAL(%s)", strerror(errno));
    pthread_detach(tid);

    memset(&req, 0, sizeof(req));
    req.tv_sec = 3;
    req.tv_nsec = 0;
    for (;;) {
        nanosleep(&req, NULL);
        ALOGE("main thread: SIGALRM is %s", sigismember(&mask, SIGALRM) ? "blocked" : "unblocked");
    }
}

void my_signal_handler(int signo, siginfo_t *info, void *context) {
    ALOGE("my_signal_handler:%d", signo);
}

void *start_routing(void *data) {
    struct sigaction action;
    sigset_t mask, old_mask, wait_mask;
    struct timespec time_obj;

    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);

    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = &my_signal_handler;
    sigemptyset(&action.sa_mask);
    sigaction(SIGALRM, &action, NULL);
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);

    pthread_sigmask(SIG_BLOCK, &mask, &old_mask);
    time_obj.tv_sec = 3;
    time_obj.tv_nsec = 0;
    wait_mask = old_mask;
    sigdelset(&wait_mask, SIGALRM);
    sigdelset(&wait_mask, SIGUSR1);
    sigdelset(&wait_mask, SIGUSR2);

    for (;;) {
        ALOGE("sleep");
        nanosleep(&time_obj, NULL);
        sigemptyset(&mask);
        if (sigpending(&mask)) {
            ALOGE("sigpending FATAL(%s)", strerror(errno));
            continue;
        }

        if (sigismember(&mask, SIGALRM) || sigismember(&mask, SIGUSR1) || sigismember(&mask, SIGUSR2)) {
            ALOGE("signal pending");
            sigsuspend(&wait_mask);
        }
    }

    return NULL;
}
