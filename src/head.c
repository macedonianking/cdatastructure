/*
 * head.c
 *
 *  Created on: 2018年4月6日
 *      Author: lhb198710
 */

#include "head.h"

pgd_t swapper_pg_dir[PTRS_PER_PGDIR];
pte_t pg0[2 * PTRS_PER_PTE];
unsigned long empty_zero_page[1024];

#define SETUP_MEM_SIZE  0x00800000

static inline pmd_t *one_md_table_init(pgd_t *pgd) {
    pud_t *pud;
    pmd_t *pmd;

    pud = pud_offset(pgd, 0);
    pmd = pmd_offset(pud, 0);
    return pmd;
}

static inline void one_pgtable_init(pte_t *pte, unsigned long pa) {
    int i;

    for (i = 0; i < PTRS_PER_PTE; ++i) {
        set_pte(pte, pfn_pte(pa >> PAGE_SHIFT, __pgprot(_KERNPG_TABLE)));
        ++pte;
        pa += PAGE_SIZE;
    }
}

/**
 * 初始的8Mb内存的映射
 */
void startup_32() {
    pgd_t *pgd;
    pmd_t *pmd, pmdval;
    pte_t *pte;
    int i;
    unsigned long addr, pa;

    pgd = swapper_pg_dir;
    for (i = 0; i < PTRS_PER_PGDIR; ++i) {
        set_pgd(pgd, __pgd(0));
        ++pgd;
    }
    addr = PAGE_OFFSET;
    pte = pg0;
    pa = 0;
    while (addr < PAGE_OFFSET + SETUP_MEM_SIZE) {
        pmdval = pfn_pmd(__pa(pte) >> PAGE_SHIFT, __pgprot(_KERNPG_TABLE));
        one_pgtable_init(pte, pa);

        pgd = swapper_pg_dir + pgd_index(addr);
        pmd = one_md_table_init(pgd);
        set_pmd(pmd, pmdval);
        pgd = swapper_pg_dir + pgd_index(addr - PAGE_OFFSET);
        pmd = one_md_table_init(pgd);
        set_pmd(pmd, pmdval);

        addr += PGDIR_SIZE;
        pa += PGDIR_SIZE;
        pte += PTRS_PER_PTE;
    }
}
