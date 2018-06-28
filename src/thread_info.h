/*
 * thread_info.h
 *
 *  Created on: 2018年2月1日
 *      Author: lhb198710
 */

#ifndef THREAD_INFO_H_
#define THREAD_INFO_H_

struct task_struct;

#define TIF_NEED_RESCHED    0x0001

struct thread_info {
    struct task_struct *task;
    unsigned int preempt_count;
    unsigned int softirq_count;
    unsigned int hardirq_count;
    unsigned long flags;
};

#define PREEMPT_ACTIVE  0x10000000

#ifdef CONFIG_4KSTACKS
#define THREAD_SIZE 4096
#else
#define THREAD_SIZE 8192
#endif

static inline struct thread_info *current_thread_info() {
    struct thread_info *t;
    asm volatile("andl %%esp, %0"
            : "=r"(t)
            : "0"(~(THREAD_SIZE-1)));
    return t;
}

static inline int test_thread_flag(int flag) {
    // TODO:
    return 1;
}

#endif /* THREAD_INFO_H_ */
