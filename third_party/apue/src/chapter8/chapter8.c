#include "chapter8.h"

#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include "utils/apue.h"
#include "utils/log.h"
#include "utils/list.h"
#include "utils/string_util.h"

static void print_wait_status(int status);

int chapter8_main(int argc, char **argv) {
    chapter8_8(argc, argv);
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

void chapter8_6(int argc, char **argv) {
    pid_t child;
    pid_t wait_child;
    int status;
    pthread_mutex_t mutex;

    if ((child = fork()) < 0) {
        APUE_ERR_SYS();
    } else if (!child) {
        /**
         * child process.
         */
        pthread_mutex_init(&mutex, NULL);
        pthread_mutex_lock(&mutex);
        pthread_mutex_lock(&mutex);
        pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&mutex);
        pthread_mutex_destroy(&mutex);
        exit(0);
    }

    wait_child = wait(&status);
    if (wait_child < 0) {
        APUE_ERR_SYS();
    }
    print_wait_status(status);
}

void print_wait_status(int status) {
    if (WIFEXITED(status)) {
        fprintf(stdout, "normal exit: exit_code=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        fprintf(stdout, "signal stop: term signal=%d, %s\n",
            WTERMSIG(status),
            WCOREDUMP(status) ? "core dump" : "dont't core dump");
    } else if (WIFSTOPPED(status)) {
        fprintf(stdout, "job stop: signal=%d\n", WSTOPSIG(status));
    } else if (WIFCONTINUED(status)) {
        fprintf(stdout, "job continue\n");
    }
}

void chapter8_7(int argc, char **argv) {
    siginfo_t siginfo_obj;
    int r;

    r = waitid(P_ALL, 0, &siginfo_obj, WEXITED | WNOHANG);
    if (r) {
        if (errno == ECHILD) {
            fprintf(stdout, "%s\n", "ECHILD");
        }
    }
}

static void chapter8_8_find_file() {
    execlp("find", "find", ".", "-name", "*.[ch]", (char*) NULL);
}

static void read_find_result(int fd, list_head *list) {
    string_node_t *str_node;
    FILE *file;
    char *line;
    size_t line_length;
    ssize_t n;

    if (!(file = fdopen(fd, "w+"))) {
        APUE_ERR_SYS();
    }

    line = NULL;
    line_length = 0;
    while ((n = getline(&line, &line_length, file)) != -1) {
        if (n > 0 && line[n-1] == '\n') {
            line[n-1] = '\0';
        }
        str_node = alloc_string_node(line);
        list_add_tail(&str_node->node, list);
    }

    free(line);
    fclose(file);
}

static void sort_find_result(struct list_head *list) {
}

static void print_find_result(struct list_head *list) {
}

static void free_find_result(struct list_head *list) {
    struct string_node_t *ptr;

    LIST_FOR_EACH_ENTRY_SAFE(ptr, list, node) {
        list_del(&ptr->node);
        free_string_node(ptr);
    }
}

void chapter8_8(int argc, char **argv) {
    char buf[PATH_MAX] = "/tmp/temp-XXXXXX";
    DEFINE_LIST_HEAD(list);
    int fd;
    int child;
    int status;

    fd = mkstemp(buf);
    if (fd < 0) {
        err_sys("can't ceate temperary file in /tmp directory: ");
        return;
    }

    if ((child = fork()) < 0) {
        APUE_ERR_SYS();
    } else if (!child) {
        /**
         * child process.
         */
        dup2(fd, STDOUT_FILENO);
        close(fd);
        chapter8_8_find_file();
        return;
    }

    if (waitpid(child, &status, 0) < 0) {
        APUE_ERR_SYS();
    }
    if (WIFEXITED(status) || WEXITSTATUS(status)) {
        exit(-1);
    }

    read_find_result(fd, &list);
    if (!list_empty(&list)) {
        sort_find_result(&list);
        print_find_result(&list);
        free_find_result(&list);
    }
    unlink(buf);
}
