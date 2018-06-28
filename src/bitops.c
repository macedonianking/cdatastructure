/*
 * bitops.c
 *
 *  Created on: 2018年4月30日
 *      Author: lhb198710
 */

#include "bitops.h"

int find_next_zero_bit(const unsigned long *addr, unsigned int size, unsigned int offset) {
    unsigned long *p = (unsigned long *) (addr + (offset >> 5));
    unsigned long set, bit = offset & 31;

    set = 0;
    if (bit) {
        asm volatile("bsfl %1, %0\n\t"
                "jne 1f\n\t"
                "movl $32, %0\n\t"
                "1:"
                : "=&r"(set)
                : "r"(~(*p >> bit))
                : "memory");
        if (set < (32 - bit)) {
            return offset + 32;
        }
        ++p;
        offset += 32 - bit;
    }

    set = find_first_zero_bit(p, size - offset);
    return offset + set;
}
