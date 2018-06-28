/*
 * pgtable-2levels.h
 *
 *  Created on: 2018年4月1日
 *      Author: lhb198710
 */

#ifndef PGTABLE_2LEVELS_H_
#define PGTABLE_2LEVELS_H_

#include "page.h"

#define PGDIR_SHIFT         22
#define PTRS_PER_PGDIR      1024

#define PTRS_PER_PTE        1024

typedef struct {
    pud_t pmd;
} pmd_t;

#define pmd_val(x)  pud_val((x).pmd)
#define __pmd(x)    ((pmd_t) { __pud(x) })

#define pte_none(x)             (!(x).pte_low)
#define pte_same(a, b)          ((a).pte_low == (b).pte_low)

#define set_pte(pteptr, pteval)         ; do { *(pteptr) = (pteval); } while (0)
#define set_pte_atomic(pteptr, pteval)  set_pte(pteptr, pteval)

#define pte_pfn(pte)                    (pte_val(pte) >> PAGE_SHIFT)
#define pfn_pte(pfn, pgprot)            __pte(((pfn)<<PAGE_SHIFT) | pgprot_val(pgprot))

#define set_pmd(pmdptr, pmdval)         ; do { *(pmdptr) = (pmdval); } while(0)
#define set_pud(pudptr, pudval)         ; do { *(pudptr) = (pudval); } while(0)

#define ptep_get_and_clear(pteptr)      __pte(xchg(&(pteptr)->pte_low, 0))

#define pud_page(pud)                   pmd_page((pmd_t){(pud)})

static inline pmd_t* pmd_offset(pud_t *pud, unsigned long addr) {
    return (pmd_t*) pud;
}

#endif /* PGTABLE_2LEVELS_H_ */
