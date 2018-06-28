/*
 * mm.h
 *
 *  Created on: 2018年4月5日
 *      Author: lhb198710
 */

#ifndef MM_H_
#define MM_H_

#include "config.h"
#include "page.h"
#include "pgtable.h"
#include "linkage.h"

struct page {
};

struct vma_area_struct {
};

struct mm_struct;
extern struct page *mem_map;
extern unsigned long max_mapnr;
extern struct mm_struct init_mm;

extern pte_t* FASTCALL(pte_alloc_map(struct mm_struct*, pmd_t*, unsigned long));
extern pte_t* FASTCALL(pte_alloc_kernel(struct mm_struct*, pmd_t*, unsigned long));

static inline pud_t *pud_alloc(struct mm_struct *mm, pgd_t *pgd, unsigned long addr) {
    return pud_offset(pgd, addr);
}

static inline pmd_t *pmd_alloc(struct mm_struct *mm, pud_t *pud, unsigned long addr) {
    return pmd_offset(pud, addr);
}

#endif /* MM_H_ */
