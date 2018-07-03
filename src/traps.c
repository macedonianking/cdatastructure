/*
 * traps.c
 *
 *  Created on: 2018年7月2日
 *      Author: 80050136
 */

#include "processor.h"
#include "config.h"
#include "segment.h"
#include "thread_info.h"
#include "printk.h"
#include "current.h"
#include "sched.h"

struct desc_struct __attribute__((section(".data.idt"))) idt_table[256];

/**
 * 可用的栈指针
 */
static inline int valid_stack_ptr(struct thread_info *tinfo, void *p) {
    return p > (void*) tinfo && p < (void*) tinfo + THREAD_SIZE - 3;
}

static unsigned long print_context_stack(struct thread_info *tinfo, unsigned long *stack,
        unsigned long ebp) {
    unsigned long addr;
#ifdef CONFIG_FRAME_POINTER
    while (valid_stack_ptr(tinfo, (void*) ebp)) {
        addr = *(unsigned long*) (ebp + 4);
        printk(" [<%08lx>] ", addr);
        printk("\n");
        ebp = *(unsigned long*) ebp;
    }
#else
    while (valid_stack_ptr(tinfo, (void*) stack)) {
        addr = *stack++;
        if (__kernel_text_address(addr)) {
            printk(" [<%08lx>] ");
            printk("\n");
        }
    }
#endif
    return ebp;
}

/**
 * 输出堆栈，考虑了双堆栈的问题
 */
void show_trace(struct task_struct *task, unsigned long *stack) {
    unsigned long ebp;

    if (!task) {
        task = current;
    }
    if (task == current) {
        asm volatile("movl %%ebp, %0":"=r"(ebp));
    } else {
        ebp = *(unsigned long *) task->thread.esp;
    }

    struct thread_info *context;
    for (;;) {
        context = (struct thread_info*) (((unsigned long) stack) & ~(THREAD_SIZE - 1));
        ebp = print_context_stack(context, stack, ebp);
        stack = (unsigned long*) context->previous_esp;
        if (!stack) {
            break;
        }
        printk("=================================\n");
    }
}

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
