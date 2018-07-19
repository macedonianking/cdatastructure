/*
 * ptrace.h
 *
 *  Created on: 2018年7月8日
 *      Author: lhb
 */

#ifndef PTRACE_H
#define PTRACE_H

struct pt_regs {
    long ebx;
    long ecx;
    long edx;
    long esi;
    long edi;
    long ebp;
    long eax;
    int xds;
    int xes;
    long orig_eax;
    long eip;
    int xcs;
    long eflags;
    long esp;
    int xss;
};

#endif //
