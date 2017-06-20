#include "chapter7.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread/thread_loop.h"
#include "process/process_util.h"
#include "utils/apue.h"
#include "utils/list.h"
#include "utils/log.h"
#include "utils/string_util.h"

int chapter7_main(int argc, char **argv) {
    chapter7_5(argc, argv);
    return 0;
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

