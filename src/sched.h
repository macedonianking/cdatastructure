/*
 * sched.h
 *
 *  Created on: 2018年4月5日
 *      Author: lhb198710
 */

#ifndef SCHED_H_
#define SCHED_H_

#include "config.h"
#include "spinlock.h"
#include "current.h"
#include "list.h"
#include "types.h"
#include "thread_info.h"
#include "pid.h"
#include "resource.h"

#define TASK_RUNNING                0
#define TASK_INTERRUPTIBLE          1
#define TASK_UNINTERRUPTIBLE        2
#define TASK_STOPPED                4
#define TASK_TRACED                 8
#define EXIT_ZOMBIE                 16
#define EXIT_DEAD                   32

#define __set_task_state(tsk, new_state)    ; do { (tsk)->state = (new_state); } while(0)
#define set_task_state(tsk, new_state)      ; do { set_mb((tsk)->state, new_state); } while(0)

#define __set_current_state(new_state)      ; do { current->state = new_state; } while(0)
#define set_current_state(new_state)        ; do { set_mb(current->state, new_state); } while(0)

struct prio_array;
typedef struct prio_array prio_array_t;

union thread_union {
    struct thread_info thread_info;
    unsigned long stack[THREAD_SIZE / sizeof(unsigned long)];
};

struct signal_struct {
    pid_t pgrp;
    pid_t session;

    struct rlimit rlim[RLIM_NLIMITS];
};

struct task_struct {
    volatile long state;
    long exit_state;

    pid_t pid;
    pid_t tpid;

    struct list_head tasks;

    int prio, static_prio;
    struct list_head run_list;
    prio_array_t *array;

    struct task_struct *real_parent;
    struct task_struct *parent;
    struct list_head children;
    struct list_head sibling;

    struct task_struct *group_leader;
    struct list_head ptrace_children;
    struct list_head ptrace_list;

    struct pid pids[PIDTYPE_MAX];

    struct thread_struct thread;
};

typedef struct task_struct task_struct;

struct mm_struct {
    spinlock_t page_table_lock;
};

#define MAX_USER_RT_PRIO            100
#define MAX_RT_PRIO                 MAX_USER_RT_PRIO
#define MAX_PRIO                    (MAX_RT_PRIO + 40)

extern struct task_struct init_task;

#define next_task(p) list_entry((p)->tasks.next, struct task_struct, tasks)
#define prev_task(p) list_entry((p)->tasks.prev, struct task_struct, tasks)

#define for_each_process(p) \
    for (p = &init_task; (p = next_task(p)) != &init_task;)

extern int try_to_wake_up(struct task_struct *task, unsigned int mode, int sync);
/**
 * 主动放弃cpu的执行，即使设置了irq的状态
 */
extern void schedule();
extern long schedule_timeout(long timeout);

#endif /* SCHED_H_ */
