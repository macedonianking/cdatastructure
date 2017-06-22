#include "process/process_util.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

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

void describe_wait_status(int status) {
    if (WIFEXITED(status)) {
        fprintf(stdout, "normal exit termination: exit_code=%d, core_dump=%d\n",
            WEXITSTATUS(status), WCOREDUMP(status));
    } else if (WIFSIGNALED(status)) {
        fprintf(stdout, "signal termination: signal=%d\n", WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
        fprintf(stdout, "stopped: signal=%d\n", WSTOPSIG(status));
    } else if (WIFCONTINUED(status)) {
        fprintf(stdout, "process continued\n");
    }
}
