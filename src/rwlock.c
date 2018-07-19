/*
 * rwlock.c
 *
 *  Created on: 2018年6月6日
 *      Author: lhb198710
 */

#include "rwlock.h"
#include "preempt.h"
#include "system.h"
#include "processor.h"

/**
 * 得到读锁
 */
static inline int __raw_read_trylock(rwlock_t *lock) {
    int r;
    asm volatile(
            "lock; decl %0;"
            "jns 1f;"
            "lock; incl %0;"
            "xorl %1, %1;"
            "1:"
            : "=m"(lock->lock), "=r"(r)
            : "1"(1)
            : "memory");
    return r;
}

#ifdef CONFIG_PREEMPT
#define __READ_WAIT_RELAX(ptr) do {\
    (ptr)->break_lock = 1; \
    while (!read_can_lock(ptr) && (ptr)->break_lock) { \
        cpu_relax(); \
    } \
} while (0)
#define __WRITE_WAIT_RELAX(ptr) do {\
    (ptr)->break_lock = 1; \
    while (!write_can_lock(ptr) && (ptr)->break_lock) { \
        cpu_relax(); \
    } \
} while (0)
#else
#define __READ_WAIT_RELAX(ptr) do {\
    while (!read_can_lock(ptr)) { \
        cpu_relax(); \
    } \
} while (0)
#define __WRITE_WAIT_RELAX(ptr) do {\
    while (!write_can_lock(ptr)) { \
        cpu_relax(); \
    } \
} while (0)
#endif

static inline void __raw_read_lock(rwlock_t *lock) {
    asm volatile("1:;"
            "lock; decl %0;"
            "jge 3f;"
            "lock; incl %0;"
            "2:;"
            "pause;"
            "cmpl $0, %0;"
            "jle 2b;"
            "jmp 1b;"
            "3:"
            : "=m"(lock->lock)
            :
            : "memory");
}

static inline void __raw_read_unlock(rwlock_t *lock) {
    asm volatile("lock; incl %0"
            : "=m"(lock->lock)
            :
            : "memory");
}

static inline int __raw_write_trylock(rwlock_t *lock) {
    int r;
    asm volatile("lock; subl $" RW_LOCK_BIAS_STR ", %0;"
            "jz 1f;"
            "lock; addl $" RW_LOCK_BIAS_STR ", %0;"
            "xorl %1, %1;"
            "1:"
            : "=m"(lock->lock), "=r"(r)
            : "1"(1)
            : "memory");
    return r;
}

static inline void __raw_write_lock(rwlock_t *lock) {
    asm volatile("1:;"
            "lock; subl $" RW_LOCK_BIAS_STR ", %0;"
            "jz 3f;"
            "lock; addl $" RW_LOCK_BIAS_STR ", %0;"
            "2:"
            "pause;"
            "cmpl $0x01000000, %0;"
            "jne 2b;"
            "jmp 1b;"
            "3:;"
            : "=m"(lock->lock)
            :
            : "memory");
}

static inline void __raw_write_unlock(rwlock_t *lock) {
    asm volatile("lock; addl $" RW_LOCK_BIAS_STR ", %0;"
            : "=m"(lock)
            :
            : "memory");
}

/**
 * 读写锁，得到读锁
 */
int read_trylock(rwlock_t *lock) {
    preempt_disable();
    if (__raw_read_trylock(lock)) {
        return 1;
    }
    preempt_enable();
    return 0;
}

void read_lock(rwlock_t *lock) {
    preempt_disable();
    for (;;) {
        if (__raw_read_trylock(lock)) {
            return;
        }

        preempt_enable(); //
        __READ_WAIT_RELAX(lock); //
        preempt_disable(); //
    }
}

void read_lock_irq(rwlock_t *lock) {
    _read_lock_irqsave(lock);
}

/**
 * 得到读锁，如果需要等待，那么循环等待的时候打开抢占的开关.得到读锁之前，抢占开关是关闭的，保证得到读锁的
 * 的时刻，读锁的是关闭的。
 * @param lock:
 */
unsigned long _read_lock_irqsave(rwlock_t *lock) { // @suppress("No return")
    unsigned long flags;

    preempt_disable();
    for (;;) {
        local_irq_save(flags);
        if (__raw_read_trylock(lock)) {
            return flags;
        }
        local_irq_restore(flags);

        preempt_enable(); //
        __READ_WAIT_RELAX(lock); //
        preempt_disable(); //
    }
}

void read_unlock(rwlock_t *lock) {
    __raw_read_unlock(lock);
    preempt_enable();
}

void read_unlock_irq(rwlock_t *lock) {
    __raw_read_unlock(lock); //
    local_irq_enable(); //
    preempt_enable(); //
}

void _read_unlock_irq(rwlock_t *lock) {
    __raw_read_unlock(lock);
    local_irq_enable();
    preempt_enable();
}

void read_unlock_irqrestore(rwlock_t *lock, unsigned long flags) {
    __raw_read_unlock(lock);
    local_irq_restore(flags);
    preempt_enable();
}

int write_trylock(rwlock_t *lock) {
    preempt_disable();
    if (__raw_write_trylock(lock)) {
        return 1;
    }
    preempt_enable();
    return 0;
}

void write_lock(rwlock_t *lock) {
    preempt_disable();
    for (;;) {
        if (__raw_write_trylock(lock)) {
            return;
        }

        preempt_enable();
        __WRITE_WAIT_RELAX(lock);
        preempt_disable();
    }
}

void write_lock_irq(rwlock_t *lock) {
    _write_lock_irqsave(lock);
}

unsigned long _write_lock_irqsave(rwlock_t *lock) { // @suppress("No return")
    unsigned long flags;

    preempt_disable();
    for (;;) {
        local_irq_save(flags);
        if (__raw_write_trylock(lock)) {
            return flags;
        }
        local_irq_restore(flags);

        preempt_enable(); //
        __WRITE_WAIT_RELAX(lock); //
        preempt_disable(); //
    }
}

void write_unlock(rwlock_t *lock) {
    __raw_write_unlock(lock);
    preempt_enable();
}

void write_unlock_irq(rwlock_t *lock) {
    __raw_write_unlock(lock);
    local_irq_enable();
    preempt_enable();
}

void write_unlock_irqrestore(rwlock_t *lock, unsigned long flags) {
    __raw_write_unlock(lock);
    local_irq_restore(flags);
    preempt_enable();
}
