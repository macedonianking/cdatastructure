/*
 * system.h
 *
 *  Created on: 2018年4月1日
 *      Author: lhb198710
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#define smp_wmb()

struct xg_dummy {
    unsigned long x[100];
};
#define __xg(ptr) ((struct xg_dummy*)(ptr))

static inline void __set_64bit(unsigned long long *ptr, unsigned int low, unsigned high) {
    asm volatile(
            "1:\n\t"
            "movl (%0), %%eax\n\t"
            "movl 4(%0), %%edx\n\t"
            "lock cmpxchg8b\n\t"
            "jnz 1b"
            :
            : "D"(ptr), "b"(low), "c"(high)
            : "eax", "edx", "memory");
}

#define ll_low(var)     (*((unsigned int *)&(var)))
#define ll_high(var)    (*((unsigned int *)&(var)) + 1)

/**
 * 常亮的优化
 */
static inline void __set_64bit_constant(unsigned long long *ptr, unsigned long long var) {
    __set_64bit(ptr, (unsigned int) var, (unsigned int) (var >> 32));
}

/**
 * 优化，省略中间的一些计算过程
 */
static inline void __set_64bit_variable(unsigned long long *ptr, unsigned long long var) {
    __set_64bit(ptr, ll_low(var), ll_high(var));
}

#define set_64bit(ptr, var) \
(__builtin_constant_p(var) ? \
 __set_64constant(ptr, var) : \
 __set_64variable(ptr, var))

static inline unsigned long __xchg(unsigned long val, unsigned long *addr, int size) {
    switch (size) {
        case 1: {
            asm volatile(
                    "xchgb %%al, %1"
                    : "+a"(val)
                    : "m"(*__xg(addr))
                    : "memory");
            break;
        }
        case 2: {
            asm volatile(
                    "xchgw %%ax, %1"
                    : "+a"(val)
                    : "m"(*__xg(addr))
                    : "memory");
            break;
        }
        case 4: {
            asm volatile(
                    "xchgl %%eax, %1"
                    : "+a"(val)
                    : "m"(*__xg(addr))
                    : "memory");
            break;
        }
    }
    return val;
}

#define xchg(ptr, val) ((typeof(*(ptr))) __xchg(((unsigned long)val), ((unsigned long*)(ptr)), sizeof(*(ptr))))

#ifdef CONFIG_SMP
#define set_mb(var, value)      ; do { xchg(&(var), value); } while(0)
#else
#define set_mb(var, value)      ; do { (var) = (value); barrier(); } while(0)
#endif

#define local_irq_flags(flags) do { \
    asm volatile("pushfl;" \
                 "popl %0" \
                 : "=r"(flags) \
                 : \
                 : "memory"); \
} while(0)

#define irq_disabled() ({ \
    unsigned long flags; \
    local_irq_flags(flags); \
    !(flags & (1 << 9)); \
})

#define local_irq_save(flags) do { \
    asm volatile("pushfl; popl %0; cli" \
        : "=g"(flags) \
        ::"memory"); \
} while (0)

#define local_irq_restore(flags) do { \
    asm volatile("pushl %0; popfl;" \
        : \
        : "r"(flags) \
        : "memory", "cc"); \
}while (0)

#define local_irq_disable() ; do { asm volatile("cli":::"memory"); } while (0)

#define local_irq_enable()  ; do { asm volatile("sti":::"memory"); } while (0)

#endif /* SYSTEM_H_ */
