/*
 * spinlock.c
 *
 *  Created on: 2018年5月27日
 *      Author: lhb198710
 */

#include "spinlock.h"

#include "system.h"
#include "preempt.h"
#include "processor.h"

#ifdef CONFIG_PREEMPT
#define __SPIN_WAIT_RELAX(lock) do { \
    if (!(lock)->break_lock) { \
        (lock)->break_lock = 1; \
    } \
    while (spin_is_locked(lock) && (lock)->break_lock) { \
        cpu_relax(); \
    } \
} while (0)
#else
#define __SPIN_WAIT_RELAX(lock) do { \
    while (spin_is_locked(lock)) { \
        cpu_relax(); \
    } \
} while (0)
#endif

/**
 * 尝试得到锁，返回原来的值
 * @param lock:
 */
static inline int __raw_spin_trylock(spinlock_t *lock) {
    char r;
    asm volatile("lock; xchgb %1, %0"
            : "=m"(lock->slock), "=q"(r)
            : "1"(0)
            : "memory");
    return r > 0;
}

/**
 * 释放锁
 * @param lock: The spinlock object pointer.
 */
static inline void __raw_spin_unlock(spinlock_t *lock) {
#ifdef CONFIG_DEBUG_SPINLOCK
    BUG_ON(lock->magic != SPINLOCK_MAGIC);
    BUG_ON(!spin_is_locked(lock));
#endif
    char r = 1;
    asm volatile(
            "xchgb %1, %0"
            : "=m"(lock->slock), "=q"(r)
            : "1"(r)
            : "memory");
}

/**
 * 先禁止抢占，如果得到锁了，直接返回，否则恢复抢占的效果设置
 * @param lock:
 */
int spin_trylock(spinlock_t *lock) {
    preempt_disable(); // 禁止抢占
    if (__raw_spin_trylock(lock)) {
        return 1;
    }
    preempt_enable();
    return 0;
}

void spin_lock(spinlock_t *lock) {
    preempt_disable();
    for (;;) {
        if (__raw_spin_trylock(lock)) {
            break;
        }

        preempt_enable();
        __SPIN_WAIT_RELAX(lock);
        preempt_disable();
    }
}

void spin_unlock(spinlock_t *lock) {
    __raw_spin_unlock(lock);
    preempt_enable();
}

/**
 * 禁止中断，禁止抢占，如果在获取锁的过程中循环，那么根据flags的条件开启中断
 * @param lock:
 * @return flags:
 */
unsigned long _spin_lock_irqsave(spinlock_t *lock) {
    unsigned long flags;

    preempt_disable();
    for (;;) {
        local_irq_save(flags);
        if (__raw_spin_trylock(lock)) {
            break;
        }
        local_irq_restore(flags);

        preempt_enable();
        if (!lock->break_lock) {
            lock->break_lock = 1;
        }
        while (spin_is_locked(lock) && lock->break_lock) {
            cpu_relax();
        }
        preempt_disable();
    }
    return 0;
}

void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags) {
    __raw_spin_unlock(lock);
    local_irq_restore(flags);
    preempt_enable();
}

void spin_unlock_irq(spinlock_t *lock) {
    __raw_spin_unlock(lock); //
    local_irq_enable(); //
    preempt_enable(); //
}

