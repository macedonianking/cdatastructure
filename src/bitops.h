/*
 * bitops.h
 *
 *  Created on: 2018年4月5日
 *      Author: lhb198710
 */

#ifndef BITOPS_H_
#define BITOPS_H_

#include "config.h"

#ifdef CONFIG_SMP
#define LOCK_PREFIX "lock;"
#else
#define LOCK_PREFIX 
#endif

#define ADDR    (*(long volatile*)(addr))

static inline void set_bit(int nr, unsigned long *addr) {
    asm volatile(LOCK_PREFIX "btsl %1, %0"
            : "=m"(ADDR)
            : "Ir"(nr)
    );
}

static inline void __set_bit(int nr, unsigned long *addr) {
    asm volatile("btsl %1, %0"
            : "=m"(ADDR)
            : "Ir"(nr));
}

static inline void clear_bit(int nr, unsigned long *addr) {
    asm volatile(LOCK_PREFIX "btrl %1, %0"
            : "=m"(ADDR)
            : "Ir"(nr));
}

static inline void __clear_bit(int nr, unsigned long *addr) {
    asm volatile("btrl %1, %0"
            : "=m"(ADDR)
            : "Ir"(nr));
}

static inline int test_and_clear_bit(int nr, unsigned long *addr) {
    int old;
    asm volatile(LOCK_PREFIX "btrl %2, %0"
            "\n\tsetc %%al"
            "\n\tmovsz %%al, %%eax"
            : "=m"(ADDR), "=a"(old)
            : "Ir"(nr)
            : "memory");
    return old;
}

static inline int __test_and_clear_bit(int nr, unsigned long *addr) {
    int old;
    asm volatile("btrl %2, %0"
            "\n\tsetc %%al"
            "\n\tmovxz %%al, %%eax"
            :"=m"(ADDR), "=a"(old)
            : "Ir"(nr)
            : "memory");
    return old;
}

/**
 * @addr: Start address
 * @size: bit length.
 */
static inline int find_first_zero_bit(const unsigned long *addr, unsigned size) {
    int d0, d1, d2;
    int res;

    if (!size)
        return 0;
    /* This looks at memory. Mark it volatile to tell gcc not to move it around */
    __asm__ __volatile__(
            "movl $-1,%%eax\n\t"
            "xorl %%edx,%%edx\n\t"
            "repe; scasl\n\t"
            "je 1f\n\t"
            "xorl -4(%%edi),%%eax\n\t"
            "subl $4,%%edi\n\t"
            "bsfl %%eax,%%edx\n"
            "1:\tsubl %%ebx,%%edi\n\t"
            "shll $3,%%edi\n\t"
            "addl %%edi,%%edx"
            :"=d" (res), "=&c" (d0), "=&D" (d1), "=&a" (d2)
            :"1" ((size + 31) >> 5), "2" (addr), "b" (addr) : "memory");
    return res;
}

extern int find_next_zero_bit(const unsigned long *addr, unsigned int size, unsigned int offset);

static inline int __test_bit_constant(int nr, unsigned long *addr) {
    return (1 << (nr & 31)) & addr[nr >> 5];
}

#endif /* BITOPS_H_ */
