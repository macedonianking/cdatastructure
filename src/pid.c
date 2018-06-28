/*
 * pid.c
 *
 *  Created on: 2018年4月30日
 *      Author: lhb198710
 */

#include "sched.h"
#include "page.h"
#include "threads.h"
#include "bitops.h"
#include "atomic.h"
#include "hash.h"
#include "init.h"
#include "atomic.h"

static struct hlist_head *pid_hash[PIDTYPE_MAX];
#define pid_hashfn(nr) hash_long((unsigned long)nr, pidhash_shift)

static int pidhash_shift;

#define PIDMAP_ENTRIES      ((PID_MAX_LIMIT + PAGE_SIZE * 8 - 1) / PAGE_SIZE / 8)
#define BITS_PER_PAGE       (PAGE_SIZE * 8)
#define BITS_PER_PAGE_MASK  (BITS_PER_PAGE - 1)
#define mk_pid(map, off)    ((map - pidmap_array) * BITS_PER_PAGE + (off))
#define find_next_offset(map, off) \
    find_next_zero_bit((map)->page, BITS_PER_PAGE, off)

typedef struct pidmap {
    atomic_t nr_free;
    void *page;
} pidmap_t;

static pidmap_t pidmap_array[PIDMAP_ENTRIES] = { //
        [0 ... PIDMAP_ENTRIES - 1] = { ATOMIC_INIT(BITS_PER_PAGE), NULL }, //
        };

void __init pidhash_init() {
    pidhash_shift = 10;
}

void fastcall free_pidmap(int nr) {
    pidmap_t *map = pidmap_array + nr / BITS_PER_PAGE;
    int offset = nr & BITS_PER_PAGE_MASK;

    clear_bit(offset, map->page);
    atomic_dec(&map->nr_free);
}

struct pid *find_pid(int type, int nr) {
    struct pid *pid;
    struct hlist_node *elem;

    hlist_for_each_entry(pid, elem, &pid_hash[type][pid_hashfn(nr)], pid_chain)
    {
        if (pid->nr == nr) {
            return pid;
        }
    }
    return NULL;
}

struct task_struct *find_task_by_pid_type(int type, int nr) {
    struct pid *pid;

    pid = find_pid(type, nr);
    if (!pid) {
        return NULL;
    }
    return pid_task(&pid->pid_list, type);
}

struct task_struct *find_task_by_pid(int nr) {
    return find_task_by_pid_type(PIDTYPE_PID, nr);
}

int fastcall attach_pid(struct task_struct *task, int type, int nr) {
    struct pid *task_pid;
    struct pid *pid;

    task_pid = &task->pids[type];
    pid = find_pid(type, nr);
    if (pid) {
        list_add_tail(&task_pid->pid_list, &pid->pid_list);
        INIT_HLIST_NODE(&task_pid->pid_chain);
    } else {
        hlist_add_head(&task_pid->pid_chain, &pid_hash[type][pid_hashfn(nr)]);
        INIT_LIST_HEAD(&task_pid->pid_list);
    }
    task_pid->nr = nr;
    return 0;
}

static int fastcall __unused __detach_pid(struct task_struct *task, int type) {
    struct pid *pid, *pid_next;
    int nr = 0;

    pid = &task->pids[type];
    if (!hlist_unhashed(&pid->pid_chain)) {
        hlist_del(&pid->pid_chain);

        if (list_empty(&pid->pid_list)) {
            nr = pid->nr;
        } else {
            pid_next = list_entry(pid->pid_list.next, struct pid, pid_list);
            hlist_add_head(&pid_next->pid_chain, &pid_hash[type][pid_hashfn(pid_next->nr)]);
        }
    }
    list_del(&pid->pid_list);
    pid->nr = 0;

    return nr;
}

void fastcall detach_pid(struct task_struct *task, int type) {
    int nr;
    int who;

    nr = __detach_pid(task, type);
    if (!nr) {
        return;
    }

    for (who = PIDTYPE_MAX; --who >= 0;) {
        if (who != type && find_pid(who, nr)) {
            return;
        }
    }
    free_pidmap(nr);
}
