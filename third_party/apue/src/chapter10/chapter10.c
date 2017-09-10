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
    pid_t pid;
    int r;
    struct sigaction action;

    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    r = pthread_create(&tid, NULL, &start_routing, NULL);
    ALOGE_ALWAYSE_FATAL_IF(r, "pthread_create FATAL(%s)", strerror(errno));
    pthread_detach(tid);

    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
    action.sa_sigaction = &my_signal_handler;
    sigaction(SIGCHLD, &action, NULL);

    if (!(pid = fork())) {
        for (;;) {
            pause();
        }
        exit(0);
    }

    for (;;) {
        pause();
    }
}

void my_signal_handler(int signo, siginfo_t *info, void *context) {
    ALOGE("my_signal_handler:%d", signo);
}

void *start_routing(void *data) {
    int signo;
    int r;
    sigset_t mask;

    for (;;) {
        ALOGD("before sigwait");
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        if ((r = sigwait(&mask, &signo) != 0)) {
            ALOGE("sigwait failure(%s)", strerror(r));
            continue;
        }

        ALOGD("sigwait return:%d", signo);
        if (signo == SIGCHLD) {
            exhaust_all_waitpid_events();
        }
    }
    return NULL;
}
