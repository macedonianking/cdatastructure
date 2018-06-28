/*
 * desc.c
 *
 *  Created on: 2018年3月21日
 *      Author: lhb198710
 */

#include "desc.h"

DEFINE_PER_CPU(struct desc_struct, cpu_gdt_table[GDT_ENTRIES]);

#ifndef CONFIG_SMP
struct desc_struct cpu_gdt_table;
#endif

struct Xgt_desc_struct cpu_gdt_descr[NR_CPUS];

extern void set_intr_gate(unsigned int irq, void *addr);

static inline unsigned long get_desc_base(unsigned long *desc) {
    unsigned long base;

    base = ((desc[0] >> 16) & 0x0000FFFF) | //
            ((desc[1] << 16) & 0x00ff0000) | //
            (desc[1] & 0xff000000);
    return base;
}
