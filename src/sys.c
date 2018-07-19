/*
 * sys.c
 *
 *  Created on: 2018年7月16日
 *      Author: lhb
 */

#include "notifier.h"
#include "rwlock.h"

static rwlock_t notifier_lock = RWLOCK_INIT;

int notifier_block_register(struct notifier_block **list, struct notifier_block *n) {
    write_lock(&notifier_lock);
    while (*list) {
        if (n->priority > (*list)->priority) {
            break;
        }
        list = &(*list)->next;
    }
    n->next = *list;
    *list = n;
    write_unlock(&notifier_lock);
    return 0;
}

int notifier_block_unregister(struct notifier_block **list, struct notifier_block *n) {
    write_lock(&notifier_lock);
    while (*list) {
        if (*list == n) {
            *list = n->next;
            break;
        }
        list = &(*list)->next;
    }
    write_unlock(&notifier_lock);
    return 0;
}
