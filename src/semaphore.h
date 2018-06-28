/*
 * semaphore.h
 *
 *  Created on: 2018年6月17日
 *      Author: lhb
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "atomic.h"
#include "wait.h"

struct semaphore {
    atomic_t count;
    int sleepers;
    wait_queue_head_t wait;
};

#define __SEMAPHORE_INITIALIZER(name, n) { \
    .count = ATOMIC_INIT(n), \
    .sleepers = 0, \
    .wait = __WAIT_QUEUE_HEAD_INITIALIZER((name).wait), \
}

#define __MUTEX_INITIALIZER(name) \
    __SEMAPHORE_INITIALIZER(name, 1)

#define __DECLEARE_GENERIC_SEMAPHORE(name, n) \
    struct semaphore name = __SEMAPHORE_INITIALIZER(name, n)
#define DECLARE_MUTEX(name)             __DECLEARE_GENERIC_SEMAPHORE(name, 1)
#define DECLARE_MUTEX_LOCKED(name)      __DECLEARE_GENERIC_SEMAPHORE(name, 0)

static inline void sema_init(struct semaphore *sem) {
}

#endif /* SEMAPHORE_H_ */
