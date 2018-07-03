/*
 * processor.h
 *
 *  Created on: 2018年3月21日
 *      Author: lhb198710
 */

#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "config.h"

struct desc_struct {
    unsigned long a, b;
};

#define X86_CR4_PGE         0x0080
static inline void prefetch(const void *ptr) {
}

extern unsigned long mmu_cr4_feature;

#define cpu_relax() asm volatile("pause")

struct thread_struct {
    unsigned long esp;
};

#endif /* PROCESSOR_H_ */
