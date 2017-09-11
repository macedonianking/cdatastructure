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

void chapter10_main_1(int argc, char **argv) {
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_SIGINFO | SA_RESTART | SA_NOCLDSTOP | SA_NOCLDWAIT;
    action.sa_sigaction = &my_signal_handler;
    sigemptyset(&action.sa_mask);
    sigaction(SIGCHLD, &action, NULL);
}

void my_signal_handler(int signo, siginfo_t *info, void *context) {
    ALOGE("my_signal_handler:%d", signo);
}
