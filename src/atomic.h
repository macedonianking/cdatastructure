/*
 * atomic.h
 *
 *  Created on: 2018年4月30日
 *      Author: lhb198710
 */

#ifndef ATOMIC_H_
#define ATOMIC_H_

typedef struct {
    volatile int counter;
} atomic_t;

#define ATOMIC_INIT(x)      { (x) }

#define atomic_read(v)      ((v)->counter)
#define atomic_set(v, i)    ((v)->counter = (i))

static inline void atomic_inc(atomic_t *v) {
    asm volatile("lock; incl %0"
            : "=m"(v->counter)
            : "m"(v->counter)
    );
}

static inline void atomic_dec(atomic_t *v) {
    asm volatile("lock; decl %0"
            :"=m" (v->counter)
            :"m" (v->counter));
}

static inline void atomic_add(int i, atomic_t *v) {
    asm volatile("lock; addl %1, %0"
            : "=m" (v->counter)
            : "ir"(i)
    );
}

static inline void atomic_sub(int i, atomic_t *v) {
    asm volatile("lock; subl %1, %0"
            : "=m" (v->counter)
            : "ir"(i)
    );
}

static inline int atomic_add_return(int i, atomic_t *v) {
    int _i;
    asm volatile("lock; xaddl %1, %0"
            : "=m" (v->counter), "=r"(_i)
            : "1"(i)
            : "memory"
    );
    return _i + i;
}

#endif /* ATOMIC_H_ */
