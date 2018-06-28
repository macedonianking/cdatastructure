/*
 * sched.c
 *
 *  Created on: 2018年4月30日
 *      Author: lhb198710
 */

#include "sched.h"

#include "bitops.h"
#include "wait.h"
#include "preempt.h"

#define BITMAP_SIZE     (((MAX_PRIO + 1 + 7) / 8 + sizeof(long)-1 ) / sizeof(long))

struct prio_array {
    unsigned int nr_active;
    unsigned long bitmap[BITMAP_SIZE];
    struct list_head queue[MAX_PRIO];
};

static void __unused dequeue_task(struct task_struct *p) {
    prio_array_t *array = p->array;

    array->nr_active--;
    list_del(&p->run_list);
    if (list_empty(array->queue + p->prio)) {
        __clear_bit(p->prio, array->bitmap);
    }
}

static void __unused enqueue_task(struct task_struct *p, prio_array_t *array) {
    list_add_tail(&p->run_list, &array->queue[p->prio]);
    __set_bit(p->prio, array->bitmap);
    ++array->nr_active;
    p->array = array;
}

// TODO:
int try_to_wake_up(struct task_struct *task, unsigned mode, int sync) {
    return 1;
}

int default_wake_function(wait_queue_t *wait, unsigned mode, int sync, void *key) {
    struct task_struct *task = wait->task;
    return try_to_wake_up(task, mode, sync);
}

#define SLEEP_ON_VARS   \
    wait_queue_t wait; \
    unsigned long flags; \
    init_waitqueue_entry(&wait, current);

#define SLEEP_ON_HEAD   \
    spin_lock_irqsave(&q->lock, flags); \
    __add_wait_queue(q, &wait); \
    spin_unlock(&q->lock);

#define SLEEP_ON_TAIL \
    spin_lock(&q->lock); \
    __remove_wait_queue(q, &wait); \
    spin_unlock_irqrestore(&q->lock, flags);

void fastcall sleep_on(wait_queue_head_t *q) {
    SLEEP_ON_VARS
    current->state = TASK_UNINTERRUPTIBLE;

    SLEEP_ON_HEAD
    schedule();
    SLEEP_ON_TAIL
}

void fastcall interruptible_sleep_on(wait_queue_head_t *q) {
    SLEEP_ON_VARS
    current->state = TASK_INTERRUPTIBLE;
    SLEEP_ON_HEAD
    schedule();
    SLEEP_ON_TAIL
}

long fastcall sleep_on_timeout(wait_queue_head_t *q, long timeout) {
    SLEEP_ON_VARS
    current->state = TASK_UNINTERRUPTIBLE;
    SLEEP_ON_HEAD
    timeout = schedule_timeout(timeout);
    SLEEP_ON_TAIL
    return timeout;
}

long fastcall interruptible_sleep_on_timeout(wait_queue_head_t *q, long timeout) {
    SLEEP_ON_VARS
    current->state = TASK_INTERRUPTIBLE;
    SLEEP_ON_HEAD
    timeout = schedule_timeout(timeout);
    SLEEP_ON_TAIL
    return timeout;
}

static void __wake_up_common(wait_queue_head_t *q, unsigned int mode, int nr_exclusive, int sync,
        void *key) {
    wait_queue_t *wait, *temp;

    list_for_each_entry_safe(wait, temp, &q->task_list, task_list)
    { //
        if (wait->func(wait, mode, sync, key) && //
                (wait->flags & WQ_FLAG_EXCUSIVE) && //
                (!--nr_exclusive)) {
            break;
        }
    }
}

void __wake_up(wait_queue_head_t *q, unsigned int mode, int nr_exclusive, void *key) {
    unsigned long flags;

    spin_lock_irqsave(&q->lock, flags);
    __wake_up_common(q, mode, nr_exclusive, 0, key);
    spin_unlock_irqrestore(&q->lock, flags);
}

void schedule() {
}

long schedule_timeout(long timeout) {
    return 0;
}

/**
 * 内核抢占
 */
void preempt_schedule() {
    struct thread_info *ti = current_thread_info();
    if (ti->preempt_count || irq_disabled()) {
        return;
    }

    need_resched:
    add_preempt_count(PREEMPT_ACTIVE);
    schedule();
    sub_preempt_count(PREEMPT_ACTIVE);
    barrier();
    if (test_thread_flag(TIF_NEED_RESCHED)) {
        goto need_resched;
    }
}
