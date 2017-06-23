#include "chapter10/chapter10.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "utils/apue.h"
#include "utils/log.h"
#include "utils/string_util.h"

static void default_signal_function(int signo);

int chapter10_main(int argc, char **argv) {
    chapter10_3(argc, argv);
    return 0;
}

void chapter10_3(int argc, char **argv) {
    char buffer[BUFSIZ];
    struct tm tm_obj;
    struct timespec time_val;

    DCHECK(signal(SIGUSR1, &default_signal_function) != SIG_ERR);
    DCHECK(signal(SIGUSR2, &default_signal_function) != SIG_ERR);

    for (;;) {
        clock_gettime(CLOCK_REALTIME, &time_val);
        localtime_r(&time_val.tv_sec, &tm_obj);
        strftime(buffer, BUFSIZ, "%c", &tm_obj);
        fprintf(stdout, "%s\n", buffer);

        time_val.tv_sec = 1;
        time_val.tv_nsec = 0;
        nanosleep(&time_val, NULL);
    }
}

void default_signal_function(int signo) {
#define SIG_CASE(signo) \
    case (signo): { \
        fprintf(stdout, #signo "\n"); \
        break; \
    }

    switch(signo) {
        SIG_CASE(SIGUSR1)
        SIG_CASE(SIGUSR2)
        default: {
            fprintf(stdout, "default_signal_function: %d\n", signo);
            break;
        }
    }

#undef SIG_CASE
}
