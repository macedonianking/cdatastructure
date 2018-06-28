/*
 * count_processor.h
 *
 *  Created on: 2018年4月26日
 *      Author: lhb198710
 */

#ifndef COUNT_COUNT_PROCESSOR_H_
#define COUNT_COUNT_PROCESSOR_H_

#include <stdint.h>

static inline uint64_t get_timebase() {
    uint64_t tick;
    asm volatile("rdtsc\n\t"
            "movl %%edx, 4(%%edi)\n\t"
            "movl %%eax, (%%edi)"
            :
            : "D"(&tick)
            : "%eax", "edx", "memory");
    return tick;
}

#endif /* COUNT_COUNT_PROCESSOR_H_ */
