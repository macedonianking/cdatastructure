/*
 * tlsflush.h
 *
 *  Created on: 2018年4月15日
 *      Author: lhb198710
 */

#ifndef TLBFLUSH_H_
#define TLBFLUSH_H_

#include "config.h"
#include "processor.h"
#include "mm.h"

#define __flush_tlb()  ; do { \
    unsigned int tmpreg;     \
    asm volatile("movl %%cr3, %0\n\r"   \
                 "movl %0, %%cr3"       \
                 : "=r"(tmpreg)         \
                 :                      \
                 : "memory");           \
} while(0)

#define __flush_tlb_global() ; do { \
    unsigned int tmpreg; \
    \
    asm volatile("movl %1, %%cr4\r\n" \
            "movl %%cr3, %0\r\n" \
            "movl %0, %%cr3\r\n" \
            "movl %2, %%cr4" \
            : "=&r"(tmpreg) \
            : "r"(mmu_cr4_feature & ~X86_CR4_PGE), \
            "r"(mmu_cr4_feature) \
            : "memory"); \
} while(0)

#define __flush_tlb_all()   __flush_tlb_global()

#define __flush_tlb_single(vaddr)    ; do { \
    asm volatile("invlpg %0"    \
            :   \
            : "m"(*((char*)(vaddr)))    \
            : "memory");    \
} while(0)

#define __flush_tlb_one(vaddr)  __flush_tlb_single(vaddr)

#ifndef CONFIG_SMP

#define flush_tlb()         __flush_tlb()
#define flush_tlb_all()     __flush_tlb_all()
#define local_flush_tbl()   __flush_tlb()

static inline void flush_tlb_mm(struct mm *mm) {
    if (mm == current->active_mm) {
        __flush_tlb();
    }
}

static inline void flush_tlb_page(struct vm_area_struct *vma, unsigned long addr) {
    if (vma->vm_mm == current->active_mm) {
        __flush_tlb_one(addr);
    }
}

static inline void flush_tlb_range(struct vm_area_struct *vma, unsigned long start, 
        unsigned long end) {
    if (vma->vm_mm == current->active_mm) {
        __flush_tbl();
    }
}

#else

#define local_flush_tbl()   __flush_tbl()
extern void flush_tlb_all();
extern void flush_tlb_current_task();
extern void flush_tlb_mm(struct mm_struct *mm);
extern void flush_tlb_page(struct vma_area_struct *vma, unsigned long addr);

#endif

#endif /* TLBFLUSH_H_ */
