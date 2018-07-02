/*
 * traps.c
 *
 *  Created on: 2018年7月2日
 *      Author: 80050136
 */

#include "processor.h"
#include "config.h"
#include "segment.h"

struct desc_struct __attribute__((section(".data.idt"))) idt_table[256];

#define __set_gate(gate_addr, type, dpl, addr, seg) \
({ \
    long d0, d1; \
    asm volatile("movw %%dx, %%ax;" \
            "movw %w4, %%dx;" \
            : "=m"(*(long*)(gate_addr)), "=m"(*((long*)(gate_addr)+1)), \
              "=&a"(d0), "=&d"(d1) \
            : "2"((seg) << 16), "3"((char*)(addr)), "i"(0x8000+((dpl)<<13)+((type)<<8)) \
            :); \
})

/**
 * 设置中断门，只能由外部触发，中断发生的时候关闭中断
 */
void set_intr_gate(unsigned int irq, unsigned long *addr) {
    __set_gate(idt_table+irq, 14, 0, addr, __KERNEL_CS);
}

/**
 * 设置系统中断门
 */
void set_system_intr_gate(unsigned int irq, unsigned long *addr) {
    __set_gate(idt_table+irq, 14, 3, addr, __KERNEL_CS);
}

void set_trap_gate(unsigned int irq, unsigned long *addr) {
    __set_gate(idt_table+irq, 15, 0, addr, __KERNEL_CS);
}

void set_system_gate(unsigned int irq, unsigned long *addr) {
    __set_gate(idt_table+irq, 15, 3, addr, __KERNEL_CS);
}

void set_task_gate(unsigned int irq, unsigned int gdt_entry) {
    __set_gate(idt_table+irq, 5, 0, 0, (gdt_entry << 3));
}
