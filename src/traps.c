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
#include "ptrace.h"
#include "siginfo.h"

#define SIGFPE          8

#ifdef __MACH__
#define DESC_SECTION	"DATA,idt"
#else
#define DESC_SECTION	".data.idt"
#endif
struct desc_struct __attribute__((section(DESC_SECTION))) idt_table[256];

asmlinkage void divide_error();

/**
 * 可用的栈指针
 */
static inline int valid_stack_ptr(struct thread_info *tinfo, void *p) {
    return p > (void*) tinfo && p < (void*) tinfo + THREAD_SIZE - 3;
}

/**
 * 输出一个stack空间的trace
 */
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
    struct thread_info *tinfo;

    if (task == current) {
        if (!stack) {
            stack = (unsigned long*) &stack;
        }
        asm volatile("movl %%ebp, %0":"=r"(ebp));
    } else {
        ebp = *(unsigned long*) task->thread.esp;
    }

    for (;;) {
        tinfo = (struct thread_info*) ((unsigned long) stack & ~(THREAD_SIZE - 1));
        ebp = print_context_stack(tinfo, stack, ebp);
        stack = (unsigned long*) tinfo->previous_esp;
        if (!stack) {
            break;
        }
        printk("=========================================\n");
    }
}

#define __set_gate(gate_addr, type, dpl, addr, seg) \
({ \
    asm volatile("movw %%dx, %%ax;" \
            "movw %w2, %%dx;" \
            : "=&a"(*(long*)(gate_addr)), "=&d"(*((long*)(gate_addr)+1)) \
            : "i"(0x8000+((dpl)<<13)+((type)<<8)), "0"((seg) << 16), "1"((char*)(addr)) \
            :); \
})

void set_intr_gate(unsigned int n, void *addr) {
    __set_gate(idt_table+n, 14, 0, addr, __KERNEL_CS);
}

void set_system_intr_gate(unsigned int irq, void *addr) {
    __set_gate(idt_table+irq, 14, 3, addr, __KERNEL_CS);
}

void set_trap_gate(unsigned int irq, void *addr) {
    __set_gate(idt_table+irq, 15, 0, addr, __KERNEL_CS);
}

void set_system_gate(unsigned int irq, void *addr) {
    __set_gate(idt_table+irq, 15, 3, addr, __KERNEL_CS);
}

void set_task_gate(unsigned int irq, unsigned int gdt_entry) {
    __set_gate(idt_table+irq, 5, 0, 0, (gdt_entry << 3));
}

void trap_init() {
    set_trap_gate(0, &divide_error);
}

asmlinkage void do_divide_error(struct pt_regs *regs, long error_code) {
}
