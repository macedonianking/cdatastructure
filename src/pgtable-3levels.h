/*
 * pgtable-3levels.h
 *
 *  Created on: 2018年4月1日
 *      Author: lhb198710
 */

#ifndef PGTABLE_3LEVELS_H_
#define PGTABLE_3LEVELS_H_

#include "page.h"
#include "system.h"

#define PGDIR_SHIFT 30
#define PTRS_PER_PGDIR  4

#define PMD_SHIFT   21
#define PTRS_PER_PMD    512

#define PTRS_PER_PTE    512

static inline int pte_none(pte_t pte) {
    return !pte.pte_low && !pte.pte_high;
}

static inline int pte_same(pte_t a, pte_t b) {
    return a.pte_low == b.pte_low && a.pte_high == b.pte_high;
}

static inline void set_pte(pte_t *ptep, pte_t pte) {
    ptep->pte_high = pte.pte_high;
    smp_wmb();
    ptep->pte_low = pte.pte_low;
}

#define __HAVE_ARCH_SET_PTE_ATOMIC
#define set_pte_atomic(pteptr, pteval)  \
    set_64bit((unsigned long long*)pteptr, pte_val(pteval))
#define set_pmd(pmdptr, pmdval)         \
    set_64bit((unsigned long long*) pmdptr, pmd_val(pmdval))
#define set_pud(pudptr, pudval)         \
    set_64bit((unsigned long long*)pudptr, pud_val(pudval))

static inline pte_t ptep_get_and_clear(pte_t *pteptr) {
    pte_t ret;

    ret.pte_low = xchg(&pteptr->pte_low, 0);
    ret.pte_high = pteptr->pte_high;
    pteptr->pte_high = 0;
    return ret;
}

#define pud_page(pud)           ((struct page*)__va(pud_val(pud) & PAGE_MASK))
#define pud_page_kernel(pud)    ((unsigned long)__va(pud_val(pud) & PAGE_MASK))

#define pmd_offset(pud, addr)   ((pmd_t*)pud_page(*(pud)) + pmd_index(addr))

#endif /* PGTABLE_3LEVELS_H_ */
