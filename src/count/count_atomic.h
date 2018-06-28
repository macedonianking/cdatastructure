/*
 * count_atomic.h
 *
 *  Created on: 2018年4月26日
 *      Author: lhb198710
 */

#ifndef COUNT_ATOMIC_H
#define COUNT_ATOMIC_H

#define ATOMIC_PREFIX    "lock; "

typedef struct atomic_t {
    int count;
} atomic_t;

#define ATOMIC_INIT(x)          { (x) }
#define DEFINE_ATOMIC(name)     atomic_t name = ATOMIC_INIT(0)

static inline void atomic_inc(atomic_t *ptr) {
    asm volatile(ATOMIC_PREFIX "incl %0"
            :
            : "m"(ptr->count)
            : "memory");
}

static inline void atomic_dec(atomic_t *ptr) {
    asm volatile(ATOMIC_PREFIX "decl %0"
            :
            : "m"(ptr->count)
            : "memory");
}

static inline int atomic_dec_test(atomic_t *ptr) {
    int r;
    asm volatile(ATOMIC_PREFIX "decl %1\n\t"
            "setz %%al\n\t"
            "movzx %%al, %%eax"
            : "=a"(r)
            : "m"(ptr->count)
            : "memory");
    return r;
}

#endif
