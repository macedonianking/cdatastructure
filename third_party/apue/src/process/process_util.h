#ifndef APUE_SRC_PROCESS_PROCESS_H
#define APUE_SRC_PROCESS_PROCESS_H

#include "utils/list.h"

void get_path_environ(struct list_head *list);
/**
 * Describe wait status.
 */
void describe_wait_status();

#endif
