#include "chapter7/chapter7.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "utils/apue.h"
#include "utils/log.h"
#include "utils/list.h"
#include "utils/string_util.h"
#include "process/process_util.h"

#define FILE_LOCK_NAME  "git.lock"

int chapter7_main(int argc, char **argv) {
    chapter7_5(argc, argv);
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
