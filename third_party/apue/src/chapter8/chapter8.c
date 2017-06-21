#include "chapter8.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/wait.h>

#include "utils/apue.h"
#include "utils/log.h"

int chapter8_main(int argc, char **argv) {
    chapter8_4(argc, argv);
    return 0;
}

void chapter8_2(int argc, char **argv) {
    uid_t uid, euid;

    uid = getuid();
    euid = geteuid();
    fprintf(stdout, "uid=%d, euid=%d\n", uid, euid);
}

void chapter8_3(int argc, char **argv) {
    char name[PATH_MAX];
    char buffer[BUFSIZ];
    char *args[10];
    int fd;
    pid_t child;
    int i, n;
    int fd_flags;

    snprintf(name, PATH_MAX, "tmp-XXXXXX");
    fd = mkstemp(name);
    if (fd < 0) {
        APUE_ERR_SYS();
    }

    if ((child = fork()) < 0) {
        APUE_ERR_SYS();
    } else if (child == 0) {
        dup2(fd, STDOUT_FILENO);
        close(fd);
        fd_flags = fcntl(STDOUT_FILENO, F_GETFL);
        fcntl(STDOUT_FILENO, F_SETFL, fd_flags | O_APPEND);

        i = 0;
        args[i++] = "find";
        args[i++] = ".";
        args[i++] = "-name";
        args[i++] = "*.c";
        execvp("find", args);
        return;
    }

    waitpid(child, NULL, 0);
    fd_flags = fcntl(fd, F_GETFL) & (~O_APPEND);
    fcntl(fd, F_SETFL, fd_flags);
    DCHECK(lseek(fd, 0, SEEK_SET) != -1);

    while((n = read(fd, buffer, BUFSIZ)) > 0) {
        write(STDOUT_FILENO, buffer, n);
    }
    close(fd);
    unlink(name);
}

/**
 * Demonstrate how to use vfork.
 */
void chapter8_4(int argc, char **argv) {

}
