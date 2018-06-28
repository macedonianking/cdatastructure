/*
 * pid.h
 *
 *  Created on: 2018年4月30日
 *      Author: lhb198710
 */

#ifndef PID_H_
#define PID_H_

#include "list.h"
#include "processor.h"
#include "linkage.h"

enum {
    PIDTYPE_PID, //
    PIDTYPE_TPID, //
    PIDTYPE_PGIP, //
    PIDTYPE_SID, //
    PIDTYPE_MAX, //
};

struct pid {
    int nr;
    struct hlist_node pid_chain;
    struct list_head pid_list;
};

struct task_struct;

void FASTCALL(free_pidmap(int nr));

struct task_struct *find_task_by_pid_type(int type, int nr);
struct task_struct *find_task_by_pid(int nr);

int FASTCALL(attach_pid(struct task_struct *task, int type, int nr));

#define pid_task(elem, type) list_entry(elem, struct task_struct, pids[type].pid_list)

#define do_each_task_pid(pid, type, task) \
    if((task = find_task_by_pid_type(type, pid))) { \
        prefetch(task->pids[type].pid_list.next); \
        do {

#define while_each_task_pid(pid, type, task) \
        while (task = pid_task(task->pids[type].pid_list.next, type), \
               prefetch(task->pids[type].pid_list.next), \
               hlist_unhashed(&task->pids[type].pid_chain)); \
    }

#endif /* PID_H_ */
