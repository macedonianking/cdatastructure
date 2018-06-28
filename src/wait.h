/*
 * wait.h
 *
 *  Created on: 2018年5月9日
 *      Author: lhb198710
 */

#ifndef WAIT_H_
#define WAIT_H_

#include "config.h"
#include "spinlock.h"
#include "list.h"
#include "linkage.h"

struct task_struct;

struct __wait_queue;
typedef struct __wait_queue wait_queue_t;
typedef int (*wait_queue_func_t)(wait_queue_t *wait, unsigned int mode, int sync, void *key);
int default_wake_function(wait_queue_t *wait, unsigned int mode, int sync, void *key);

#define WQ_FLAG_EXCUSIVE   0x01

struct __wait_queue {
    unsigned int flags;
    struct task_struct *task;
    wait_queue_func_t func;
    struct list_head task_list;
};

typedef struct __wait_queue_head {
    spinlock_t lock;
    struct list_head task_list;
} wait_queue_head_t;

#define __WAITQUEUE_INITIALIZER(name, tsk) { \
    .task = (tsk), \
    .func = default_wake_function, \
    .task_list = { NULL, NULL } \
}

#define DECLARE_WAITQUEUE(name, tsk) wait_queue_t name = __WAITQUEUE_INITIALIZER(name, tsk)

#define __WAIT_QUEUE_HEAD_INITIALIZER(name) { \
    .lock = SPIN_LOCK_UNLOCKED, \
    .task_list = { &(name).task_list, &(name).task_list } \
}

#define DECLARE_WAIT_QUEUE_HEAD(name) wait_queue_head_t name = __WAIT_QUEUE_HEAD_INITIALIZER(name)

static inline void init_wait_queue_head(wait_queue_head_t *q) {
    q->lock = SPIN_LOCK_UNLOCKED;
    INIT_LIST_HEAD(&q->task_list);
}

static inline void init_waitqueue_entry(wait_queue_t *q, struct task_struct *task) {
    q->flags = 0;
    q->task = task;
    q->func = default_wake_function;
}

static inline void init_waitqueue_func_entry(wait_queue_t *q, wait_queue_func_t func) {
    q->flags = 0;
    q->task = NULL;
    q->func = func;
}

#define is_sync_wait(wait) (!(wait) || (wait)->task)

extern int autoremove_wake_function(wait_queue_t *wait, unsigned int mode, int sync, void *key);
extern void FASTCALL(add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait));
extern void FASTCALL(add_wait_queue_exclusive(wait_queue_head_t *q, wait_queue_t *wait));
extern void FASTCALL(remove_wait_queue(wait_queue_head_t *q, wait_queue_t *wait));

extern void FASTCALL(sleep_on(wait_queue_head_t *q));
extern void FASTCALL(interruptible_sleep_on(wait_queue_head_t *q));
extern long FASTCALL(sleep_on_timeout(wait_queue_head_t *q, long timeout));
extern long FASTCALL(interruptible_sleep_on_timeout(wait_queue_head_t *q, long timeout));

extern void FASTCALL(prepare_to_wait(wait_queue_head_t *q, wait_queue_t *wait, int state));
extern void FASTCALL(prepare_to_wait_exclusive(wait_queue_head_t *q, wait_queue_t *wait, int state));
extern void FASTCALL(finish_wait(wait_queue_head_t *q, wait_queue_t *wait));

#define DEFINE_WAIT(name) wait_queue_t name = { \
    .task = current, \
    .func = autoremove_wake_function, \
    .task_list = { &(name).task_list, &(name).task_list }, \
}

static inline void __add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait) {
    list_add(&wait->task_list, &q->task_list);
}

static inline void __add_wait_queue_tail(wait_queue_head_t *q, wait_queue_t *wait) {
    list_add_tail(&wait->task_list, &q->task_list);
}

static inline void __remove_wait_queue(wait_queue_head_t *q, wait_queue_t *wait) {
    list_del(&wait->task_list);
}

static inline int waitqueue_active(wait_queue_head_t *q) {
    return !list_empty(&q->task_list);
}

void __wake_up(wait_queue_head_t *q, unsigned int mode, int nr_exclusive, void *key);

#define wake_up(q) __wake_up(q, TASK_UNINTERRUPTIBLE|TASK_INTERRUPTIBLE, 1, NULL)
#define wake_up_nr(q, nr) __wake_up(q, TASK_UNINTERRUPTIBLE|TASK_INTERRUPTIBLE, nr, NULL)
#define wake_up_all(q, nr) __wake_up(q, TASK_UNINTERRUPTIBLE|TASK_INTERRUPTIBLE, 0, NULL)
#define wake_up_interruptible(q) __wake_up(q, TASK_INTERRUPTIBLE, 1, NULL)
#define wake_up_nr_interruptible(q, nr) __wake_up(q, TASK_INTERRUPTIBLE, nr, NULL)
#define wake_up_all_interruptible(q, nr) __wake_up(q, TASK_INTERRUPTIBLE, 0, NULL)

#define __wait_event(q, condition) do { \
    DEFINE_WAIT(__wait); \
    \
    for (;;) { \
        prepare_to_wait(q, &__wait, TASK_UNINTERRUPTIBLE); \
        if (condition) { \
            break; \
        } \
        schedule(); \
    } \
    finish_wait(q, &__wait); \
} while (0)

#define wait_event(q, condition) do { \
    if (condition) { \
        break; \
    } \
    __wait_event(q, condition); \
} while (0)

#define __wait_event_interruptible(q, condition) do { \
    DEFINE_WAIT(__wait); \
    for (;;) { \
        prepare_to_wait(q, &__wait, TASK_INTERRUPTIBLE); \
        if (condition) { \
            break; \
        } \
        schedule(); \
    } \
    finish_wait(q, &__wait); \
} while (0)

#define wait_event_interruptible(q, condition) do { \
    if (condition) { \
        break; \
    } \
    __wait_event_interruptible(q, condition); \
} while (0)

#define __wait_event_exclusive(q, condition) do { \
    DEFINE_WAIT(__wait); \
    for (;;) { \
        prepare_to_wait_exclusive(q, &__wait, TASK_UNINTERRUPTIBLE); \
        if (condition) { \
            break; \
        } \
        schedule(); \
    } \
    finish_wait(q, &__wait); \
} while (0)

#define wait_event_exclusive(q, condition) do { \
    if (condition) { \
        break; \
    } \
    __wait_event_exclusive(q, condition); \
} while (0)

#define __wait_event_exclusive_interruptible(q, condition) do { \
    DEFINE_WAIT(__wait); \
    for (;;) { \
        prepare_to_wait_exclusive(q, &__wait, TASK_INTERRUPTIBLE); \
        if (condition) { \
            break; \
        } \
        schedule(); \
    } \
    finish_wait(q, &__wait); \
} while (0)

#define wait_event_exclusive_interrruptible(q, condition) do { \
    if (condition) { \
        break; \
    } \
    __wait_event_exclusive_interruptible(q, condition); \
} while (0)

#define __wait_event_timeout(q, condition, timeout) do { \
    DEFINE_WAIT(__wait); \
    for (;;) { \
        prepare_to_wait(q, &__wait, TASK_UNINTERRUPTIBLE); \
        if (condition) { \
            break; \
        } \
        timeout = schedule_timeout(timeout); \
        if (timeout <= 0) { \
            break; \
        } \
    } \
    finish_wait(q, &__wait); \
} while (0)

#define wait_event_timeout(q, condition, timeout) ({ \
    long __ret = timeout; \
    if (!(condition)) { \
        __wait_event_timeout(q, condition, __ret); \
    } \
    __ret; \
})

#endif /* WAIT_H_ */
