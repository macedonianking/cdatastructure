/*
 * preempt.h
 *
 *  Created on: 2018年5月27日
 *      Author: lhb198710
 */

// 控制进程切换是否被允许的状态
//
#ifndef PREEMPT_H_
#define PREEMPT_H_

#include "config.h"
#include "kernel.h"
#include "current.h"
#include "linkage.h"
#include "percpu.h"

#define preempt_count() (current_thread_info()->preempt_count)

#define add_preempt_count(val) ; do { preempt_count() += (val); } while (0)
#define sub_preempt_count(val) ; do { preempt_count() -= (val); } while (0)

#define inc_preempt_count() add_preempt_count(1)
#define dec_preempt_count() sub_preempt_count(1)

asmlinkage void preempt_schedule();

#define preempt_disable() do { \
    inc_preempt_count(); \
    barrier(); \
} while (0)

#define preempt_enable_no_resched() do { \
    barrier(); \
    dec_preempt_count(); \
} while (0)

#define preempt_check_resched() do { \
    if (test_thread_flag(TIF_NEED_RESCHED)) { \
        preempt_schedule(); \
    } \
}while (0)

#define preempt_enable() ; do { \
    preempt_enable_no_resched(); \
    preempt_check_resched(); \
} while (0)

#define get_cpu() ({ preempt_disable(); })
#define put_cpu() ({ preempt_enable(); })
#define put_cpu_no_resched() ({ preempt_enable_no_resched(); })

#define get_cpu_var(var) (*({ preempt_disable(); &__get_cpu_var(var); }))
#define put_cpu_var(var) ({ (void)(var); preempt_enable(); })

#endif /* PREEMPT_H_ */
