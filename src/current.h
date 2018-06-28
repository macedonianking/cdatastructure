/*
 * current.h
 *
 *  Created on: 2018年4月15日
 *      Author: lhb198710
 */

#ifndef CURRENT_H_
#define CURRENT_H_

#include "thread_info.h"

struct task_struct;

static inline struct task_struct *get_current() {
    return current_thread_info()->task;
}

#define current get_current()

#endif /* CURRENT_H_ */
