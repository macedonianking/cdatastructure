#include "chapter7/chapter7.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <setjmp.h>
#include <sys/resource.h>

#include "process/process_util.h"
#include "utils/apue.h"
#include "utils/list.h"
#include "utils/log.h"
#include "utils/string_buffer.h"
#include "utils/string_util.h"

#define FILE_LOCK_NAME  "git.lock"

int chapter7_main(int argc, char **argv) {
    chapter7_10(argc, argv);
    return 0;
}

static void chapter7_3_exit_function() {
    unlink(FILE_LOCK_NAME);
}

void chapter7_3(int argc, char **argv) {
    int fd;

    if ((fd = open(FILE_LOCK_NAME, O_CREAT | O_RDWR | O_TRUNC | O_EXCL, APUE_FILE_MODE)) < 0) {
        exit(-1);
    }

    close(fd);
    atexit(&chapter7_3_exit_function);

    sleep(10);
}

void chapter7_4(int argc, char **argv) {
    char **pptr;
    int i;

    pptr = argv;
    i = 0;

    while (*pptr) {
        fprintf(stdout, "argv[%d]: %s\n", i++, *pptr);
        pptr++;
    }
}

void chapter7_5(int argc, char **argv) {
    DEFINE_LIST_HEAD(list);
    string_node_t *ptr;
    int i;

    get_path_environ(&list);
    if (list_empty(&list)) {
        return;
    }

    i = 0;
    LIST_FOR_EACH_ENTRY_SAFE(ptr, &list, node) {
    list_del(&ptr->node);
        fprintf(stdout, "PATH[%d] = %s\n", i++, ptr->str);
        free_string_node(ptr);
    }
}

void chapter7_6(int argc, char **argv) {
    char buffer[PATH_MAX];
    char *path;
    string_buffer_t str_buf;

    if(!getcwd(buffer, PATH_MAX)) {
        APUE_ERR_SYS();
    }

    path = getenv("PATH");
    DCHECK(path);

    string_buffer_init(&str_buf);
    string_buffer_printf(&str_buf, "PATH=%s:%s", buffer, path);
    putenv(str_buf.buf);

    path = getenv("PATH");
    fprintf(stdout, "str_buf.buf=%p, path=%p\n", str_buf.buf, path);
}

void chapter7_9(int argc, char **argv) {
    char buffer[PATH_MAX];
    char *java_home;
    int n;

    java_home = getenv("JAVA_HOME");
    if (!java_home) {
        return;
    }

    n = strlen(java_home);
    if (n + 1 > PATH_MAX) {
        return;
    }
    strncpy(buffer, java_home, PATH_MAX - 1);
    fprintf(stdout, "JAVA_HOME=%s\n", buffer);
}

static void chapter7_10_internal(jmp_buf env) {
    longjmp(env, 10);
}

void chapter7_10(int argc, char **argv) {
    jmp_buf env;
    int r;

    r = setjmp(env);
    if (!r) {
        chapter7_10_internal(env);
    } else {
        fprintf(stdout, "r=%d\n", r);
    }
}

void chapter7_11(int argc, char **argv) {
    struct rlimit limit_obj;
}
