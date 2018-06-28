/*
 * desc.h
 *
 *  Created on: 2018年3月21日
 *      Author: lhb198710
 */

#ifndef DESC_H_
#define DESC_H_

#include "config.h"
#include "percpu.h"
#include "module.h"
#include "segment.h"
#include "processor.h"

extern struct desc_struct cpu_gdt_table[GDT_ENTRIES];
DECLARE_PER_CPU(struct desc_struct, cpu_gdt_table[GDT_ENTRIES]);

#pragma pack(push)
#pragma pack(1)

struct Xgt_desc_struct {
    unsigned short size;
    unsigned long address __attribute__((packed));
    unsigned short pad;
}__attribute__((packed));

#pragma pack(pop)

extern struct Xgt_desc_struct cpu_gdt_descr[NR_CPUS];

#endif /* DESC_H_ */
