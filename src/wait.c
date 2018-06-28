/*
 * wait.c
 *
 *  Created on: 2018年5月16日
 *      Author: lhb198710
 */

#include "wait.h"

#include "sched.h"

/**
 * 唤醒进程并且从等待列表中移除wait
 */
int autoremove_wake_function(wait_queue_t *wait, unsigned mode, int sync, void *key) {
    int ret;

    ret = default_wake_function(wait, mode, sync, key);
    if (ret) {
        /**
         * 删除后没有充值前后的指针
         */
        list_del_init(&wait->task_list);
    }
    return ret;
}

void fastcall add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait) {
    unsigned long flags;

    wait->flags &= ~WQ_FLAG_EXCUSIVE;
    spin_lock_irqsave(&q->lock, flags);
    __add_wait_queue(q, wait);
    spin_unlock_irqrestore(&q->lock, flags);
}

void fastcall add_wait_queue_exclusive(wait_queue_head_t *q, wait_queue_t *wait) {
    unsigned long flags;

    wait->flags |= WQ_FLAG_EXCUSIVE;
    spin_lock_irqsave(&q->lock, flags);
    __add_wait_queue_tail(q, wait);
    spin_unlock_irqrestore(&q->lock, flags);
}

void fastcall remove_wait_queue(wait_queue_head_t *q, wait_queue_t *wait) {
    unsigned long flags;

    spin_lock_irqsave(&q->lock, flags);
    __remove_wait_queue(q, wait);
    spin_unlock_irqrestore(&q->lock, flags);
}

void fastcall prepare_to_wait(wait_queue_head_t *q, wait_queue_t *wait, int state) {
    unsigned long flags;

    wait->flags &= ~WQ_FLAG_EXCUSIVE;
    spin_lock_irqsave(&q->lock, flags);
    if (list_empty(&wait->task_list)) {
        __add_wait_queue(q, wait);
    }

    if (is_sync_wait(wait)) {
        set_current_state(state);
    }
    spin_unlock_irqrestore(&q->lock, flags);
}

void fastcall prepare_to_wait_exclusive(wait_queue_head_t *q, wait_queue_t *wait, int state) {
    unsigned long flags;

    wait->flags |= WQ_FLAG_EXCUSIVE;
    spin_lock_irqsave(&q->lock, flags);
    if (list_empty(&wait->task_list)) {
        __add_wait_queue_tail(q, wait);
    }

    if (is_sync_wait(wait)) {
        set_current_state(state);
    }
    spin_unlock_irqrestore(&q->lock, flags);
}

void fastcall finish_wait(wait_queue_head_t *q, wait_queue_t *wait) {
    unsigned long flags;

    __set_current_state(TASK_RUNNING);
    if (!list_empty_carefuly(&wait->task_list)) {
        spin_lock_irqsave(&q->lock, flags);
        list_del_init(&wait->task_list);
        spin_unlock_irqrestore(&q->lock, flags);
    }
}
