#ifndef APUE_SRC_PROCESS_PROCESS_H
#define APUE_SRC_PROCESS_PROCESS_H

#include <unistd.h>

#include "utils/list.h"

void get_path_environ(struct list_head *list);
/**
 * Describe wait status.
 */
void describe_wait_status(pid_t pid, int status);

/**
 * exhaust
 */
void exhaust_all_waitpid_events();

#endif
