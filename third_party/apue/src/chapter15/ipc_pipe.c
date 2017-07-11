#include "chapter15/ipc_pipe.h"

#include <dirent.h>

#include "utils/apue.h"

void ipc_pipe_main(int argc, char **argv) {
    ipc_pipe_main_2(argc, argv);
}

void ipc_pipe_main_1(int argc, char **argv) {
    int pipefd[2];
    struct stat stat_obj;

    if (pipe(pipefd) == -1) {
        LOGE("pipe FATAL");
        exit(-1);
    }

    if (!fstat(pipefd[0], &stat_obj)) {
        if (S_ISFIFO(stat_obj.st_mode)) {
            LOGD("S_ISFIFO");
        }
    }

    close(pipefd[0]);
    close(pipefd[1]);
}
/*
static void list_dir(const char *dir_path, int fd) {
    DIR *dirp;
    struct dirent ent, *ret;

    if (!(dirp = opendir(dir_path))) {
        return;
    }
    while (!readdir_r(dirp, &ent, &ret) && &ent == ret) {
        dprintf(fd, "%s\n", ret->d_name);
    }
    closedir(dirp);
}

static void ipc_pipe_main_2_child_process(int fd) {
    list_dir("./out", fd);
    close(fd);
}
*/
void ipc_pipe_main_2(int argc, char **argv) {
    char buf[BUFSIZ];
    pid_t child;
    int pipefd[2];
    int fd;
    int n;

    if (pipe(pipefd) == -1) {
        LOGE("pipe FATAL");
        exit(-1);
    }
    if ((child = fork()) == -1) {
        LOGE("fork FATAL");
        exit(-1);
    } else if (!child) {
        close(pipefd[0]);
        sleep(10);
        close(pipefd[1]);
        exit(0);
    }
    close(pipefd[1]);
    fd = pipefd[0];

    for (;;) {
        if ((n = read(fd, buf, BUFSIZ)) == -1) {
            break;
        } else if (!n) {
            break;
        }
        if (writen(STDOUT_FILENO, buf, n) != n) {
            break;
        }
    }
    close(fd);
}
