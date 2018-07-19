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

struct Xgt_desc_struct {
    unsigned short size;
    unsigned long address __attribute__((packed));
    unsigned short pad;
}__attribute__((packed));

extern struct Xgt_desc_struct idt_descr, cpu_gdt_descr[NR_CPUS];

/**
 * 设置系统中断门
 */
void set_intr_gate(unsigned int irq, void *addr);
/**
 * 设置系统中断门，能够被用户态进程触发
 */
void set_system_intr_gate(unsigned int irq, void *addr);

/**
 * 设置陷阱门，只能由系统触发
 */
void set_trap_gate(unsigned int n, void *addr);
/**
 * 设置系统陷阱门，能够被用户触发
 */
void set_system_gate(unsigned int n, void *addr);

void set_task_gate(unsigned int n, unsigned int gdt_entry);

#endif /* DESC_H_ */
