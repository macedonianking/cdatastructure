/*
 * spinlock.h
 *
 *  Created on: 2018年5月10日
 *      Author: lhb198710
 */

#ifndef SRC_SPINLOCK_H_
#define SRC_SPINLOCK_H_

#include "config.h"
#include "compiler.h"
#include "linkage.h"
#include "bug.h"

int asmlinkage printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

typedef struct {
    volatile unsigned int slock;
#ifdef CONFIG_DEBUG_SPINLOCK
    unsigned int magic;
#endif
#ifdef CONFIG_PREEMPT
    unsigned int break_lock;
#endif
} spinlock_t;

#define SPINLOCK_MAGIC 0xdead4eaf

#ifdef CONFIG_DEBUG_SPINLOCK
#define SPINLOCK_MAGIC_INIT , 0xdead4eaf
#else
#define SPINLOCK_MAGIC_INIT
#endif

#define SPIN_LOCK_UNLOCKED (spinlock_t) { 1 SPINLOCK_MAGIC_INIT }

#define spin_lock_init(x) do { *(x) = SPIN_LOCK_UNLOCKED; } while (0)

#define spin_is_locked(x) (((volatile int*)((x)->slock)) <= 0)
#define spin_unlock_wait(lock) do { barrier(); } while (spin_is_locked(lock));

// 保护不会再中断函数中使用的资源，在等待的过程中可以执行中断函数，允许抢占发生
int spin_trylock(spinlock_t *lock);
void spin_lock(spinlock_t *lock);
void spin_unlock(spinlock_t *lock);
unsigned long _spin_lock_irqsave(spinlock_t *lock);
#define spin_lock_irq(lock) _spin_lock_irq(lock)
#define spin_lock_irqsave(lock, flags) do { flags = _spin_lock_irqsave(lock); } while (0)
void spin_unlock_irq(spinlock_t *lock);
void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);

#include "rwlock.h"

#endif /* SRC_SPINLOCK_H_ */
