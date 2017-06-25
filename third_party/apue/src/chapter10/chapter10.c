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
#include "utils/math_help.h"
#include "utils/string_util.h"

static void default_signal_function(int signo);

int chapter10_main(int argc, char **argv) {
    chapter10_9(argc, argv);
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

/**
 * Demonstrate how use kill api.
 */
void chapter10_9(int argc, char **argv) {
    struct timespec timeobj;
    pid_t child;
    int status;
    long millis, delta, threshold;
    int fd;

    if ((child = fork()) < 0) {
        LOGE("fork FATAL");
        exit(-1);
    } else if (!child) {
        fprintf(stdout, "child process: pid=%d, pgid=%d\n",
            getpid(), getpgid(0));
        if((fd = open("/dev/null", O_WRONLY)) < 0) {
            err_sys("can't open /dev/null");
        }
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
        execlp("find", "find", "/", "*.c", NULL);
    }

    fprintf(stdout, "parent process: pid=%d, pgid=%d\n",
        getpid(), getpgid(0));
    millis = current_time_millis();
    threshold = 10 * SECOND_IN_MILLIS;
    for (;;) {
        delta = threshold - (current_time_millis() - millis);
        delta = MAX(0, delta);
        delta = MIN(delta, threshold);
        if (delta == 0) {
            break;
        }

        timeobj.tv_sec = delta / SECOND_IN_MILLIS;
        timeobj.tv_nsec = (delta % SECOND_IN_MILLIS) * MILLIS_IN_NANOS;
        nanosleep(&timeobj, NULL);
    }
    if (kill(-getpgid(0), SIGQUIT)) {
        err_sys("kill FATAL");
    }
    if(waitpid(child, &status, 0) == child) {
        if (WIFSIGNALED(status)) {
            fprintf(stdout, "child stop for signal=%d\n, core_dump=%d",
                WTERMSIG(status),
                WCOREDUMP(status));
        }
    }

    sleep(2);
    fprintf(stdout, "parent process finish\n");
}
