/*
 * page.h
 *
 *  Created on: 2018年4月1日
 *      Author: lhb198710
 */

#ifndef PAGE_H_
#define PAGE_H_

#include "config.h"

#define PAGE_SHIFT  12
#define PAGE_SIZE   (1UL << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))
#define PTE_MASK    PAGE_MASK

#define LARGE_PAGE_MASK (~(LARGE_PAGE_SIZE-1))
#define LARGE_PAGE_SIZE (1 << PMD_SHIFT)

#ifdef CONFIG_X86_PAE
typedef struct {
    unsigned long pte_low, pte_high;
}pte_t;
typedef struct {
    unsigned long long pmd;
}pmd_t;
typedef struct {
    unsigned long long pgd;
}pgd_t;
typedef struct {
    unsigned long long pgprot;
}pgprot_t;
#define pte_val(x)  ((x).pte_low | (((unsigned long long)((x).pte_high)) << 32))
#define pmd_val(x)  ((x).pmd)
#define __pmd(x)    ((pmd_t) {(x)})
#else /* CONFIG_X86_PAE */
typedef struct {
    unsigned long pte_low;
} pte_t;
typedef struct {
    unsigned long pgd;
} pgd_t;
typedef struct {
    unsigned long pgprot;
} pgprot_t;

#define pte_val(x)  ((x).pte_low)
#endif /* !CONFIG_X86_PAE */

#define __pte(x)    ((pte_t){(x)})
#define pgd_val(x)  ((x).pgd)
#define __pgd(x)    ((pgd_t) {(x)})
#define pgprot_val(x)   ((x).pgprot)
#define __pgprot(x)     ((pgprot_t){(x)})

typedef struct {
    pgd_t pgd;
} pud_t;

#define pud_val(x)  pgd_val((x).pgd)
#define __pud(x)    ((pud_t){ __pgd(x) })

#define __PAGE_OFFSET   0xC0000000UL

#define PAGE_OFFSET     __PAGE_OFFSET
#define __pa(x)         ((unsigned long)(x) - PAGE_OFFSET)
#define __va(x)         ((void*)((x) + PAGE_OFFSET))

/**
 * pfn到虚拟地址
 */
#define pfn_to_kaddr(pfn)   __va((pfn) << PAGE_SHIFT)
/**
 * 不是非连续的内存设置
 */
#ifndef CONFIG_DISCONTIGMEM
#define pfn_to_page(pfn)        (mem_map + (pfn))
#define page_to_pfn(page)       ((page) - mem_map)
#define pfn_valid(pfn)          ((pfn) < max_memnr)
#endif

#endif /* PAGE_H_ */
