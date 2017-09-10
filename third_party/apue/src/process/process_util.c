#include "process/process_util.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

#define LOG_TAG "process_util"
#include "utils/list.h"
#include "utils/log.h"
#include "utils/string_util.h"

extern char **environ;

void get_path_environ(list_head *list) {
    char **pptr, *str;
    int n;
    const char* prefix = "PATH=";

    n = strlen(prefix);
    pptr = environ;
    while (*pptr) {
        if (!strncmp(*pptr, prefix, n)) {
            break;
        }
        ++pptr;
    }
    if (!*pptr) {
        return;
    }
    str = *pptr + n;
    string_split(str, ':', list);
}

void describe_wait_status(int pid, int status) {
    if (WIFEXITED(status)) {
        ALOGD("normal exit termination: pid=%d, exit_code=%d, core_dump=%d",
            pid, WEXITSTATUS(status), WCOREDUMP(status));
    } else if (WIFSIGNALED(status)) {
        ALOGD("signal termination: pid=%d, signo=%d",
            pid, WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
        ALOGD("stopped: pid=%d, signo=%d",
            pid, WSTOPSIG(status));
    } else if (WIFCONTINUED(status)) {
        ALOGD("process continued: pid=%d", pid);
    } else {
        ALOGD("unknown SIGCHLD event: pid=%d", pid);
    }
}

void exhaust_all_waitpid_events() {
    pid_t pid;
    int status;

    for (;;) {
        if ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) == -1) {
            if (errno == ECHILD) {
                return;
            } else if (errno != EINTR) {
                ALOGE("waitpid failure(%s)", strerror(errno));
            }
            continue;
        } else if (!pid) {
            break;
        }
        describe_wait_status(pid, status);
    }
}
