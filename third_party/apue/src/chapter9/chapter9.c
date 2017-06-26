#include "chapter9/chapter9.h"

#include "utils/apue.h"

int chapter9_main(int argc, char **argv) {
    chapter9_3(argc, argv);
    return 0;
}

/**
 *
 */
void chapter9_3(int argc, char **argv) {
    pid_t child;
    int status;

    if((child = fork()) < 0) {
        APUE_ERR_SYS();
    } else if (!child) {
        /**
         * child process.
         */
        fprintf(stdout, "child process: pid=%d, pgid=%d\n", getpid(), getpgid(0));
        exit(0);
    }

    if(waitpid(child, &status, 0) <= 0) {
        APUE_ERR_SYS();
    }
    if(WIFEXITED(status) && !WEXITSTATUS(status)) {
        fprintf(stdout, "parent process: pid=%d, pgid=%d\n", getpid(), getpgrp());
    }
}
