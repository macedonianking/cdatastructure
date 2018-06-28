/*
 * rwlock.h
 *
 *  Created on: 2018年6月6日
 *      Author: lhb198710
 */

#ifndef RWLOCK_H_
#define RWLOCK_H_

#include "compiler.h"
#include "config.h"

typedef struct {
    volatile unsigned int lock;
#ifdef CONFIG_PREEMPT
    unsigned int break_lock;
#endif
} rwlock_t;

#define RW_LOCK_BIAS        0x01000000
#define RW_LOCK_BIAS_STR    "0x01000000"

#define RWLOCK_INIT         (rwlock_t){ RW_LOCK_BIAS }
#define rwlock_init(lock)   ; do { *(lock) = RELOCK_INIT; } while (0)

#define read_can_lock(x)     ((int)(x)->lock > 0)
#define write_can_lock(x)    (((int)(x)->lock) == RW_LOCK_BIAS)
#define rwlock_is_locked(lock) ((lock)->lock != RW_LOCK_BIAS)

int read_trylock(rwlock_t *lock);
void read_lock(rwlock_t *lock);
void read_lock_irq(rwlock_t* lock);
unsigned long _read_lock_irqsave(rwlock_t* lock);
#define read_lock_irqsave(lock, flags) do { flags = _read_lock_irqsave(lock); } while (0)
void read_unlock(rwlock_t *lock);
void read_unlock_irq(rwlock_t *lock);
void read_unlock_irqrestore(rwlock_t *lock, unsigned long flags);

int write_trylock(rwlock_t *lock);
void write_lock(rwlock_t *lock);
void write_lock_irq(rwlock_t *lock);
unsigned long _write_lock_irqsave(rwlock_t *lock);
#define write_lock_irqsave(lock, flags) ; do { flags = _write_lock_irqsave(lock); }  while (0)
void write_unlock(rwlock_t *lock);
void write_unlock_irq(rwlock_t *lock);
void write_unlock_irqrestore(rwlock_t *lock, unsigned long flags);

#endif /* RWLOCK_H_ */
