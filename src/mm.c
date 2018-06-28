/*
 * mm.c
 *
 *  Created on: 2018年4月5日
 *      Author: lhb198710
 */

#include "mm.h"
#include "page.h"
#include "pgtable.h"
#include "sched.h"

struct page *mem_map = 0;
unsigned long max_mapnr;
struct mm_struct init_mm;

pte_t fastcall *pte_alloc_map(struct mm_struct *mm, pmd_t *pmd, unsigned long addr) {
    return 0;
}

pte_t fastcall *pte_alloc_kernel(struct mm_struct *mm, pmd_t *pmd, unsigned long addr) {
    return 0;
}

