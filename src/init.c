/*
 * init.c
 *
 *  Created on: 2018年4月6日
 *      Author: lhb198710
 */

#include "page.h"
#include "pgtable.h"
#include "init.h"
#include "sched.h"
#include "bootmem.h"
#include "mmzone.h"
#include "bug.h"
#include "fixmap.h"

static int __init is_kernel_text(unsigned long addr) {
    return 1;
}

/**
 * 初始化pgd->pmd
 */
static pmd_t __init *one_md_table_init(pgd_t *pgd) {
    pud_t *pud;
    pmd_t *pmd;

#ifdef CONFIG_X86_PAE
    pmd = alloc_bootmem_lowpages(PAGE_SIZE);
    set_pgd(pgd, __pgd(__pa(pmd) | _PAGE_PRESNET));
    pud = pud_offset(pgd, 0);
    if (pmd != pmd_offset(pud, 0)) {
        BUG();
    }
#else
    pud = pud_offset(pgd, 0);
    pmd = pmd_offset(pud, 0);
#endif
    return pmd;
}

/**
 * 初始化一个PAGE TABLE
 * @param pmd:
 */
static pte_t __init *one_page_table_init(pmd_t *pmd) {
    pte_t *pte;

    if (pmd_none(*pmd)) {
        pte = alloc_bootmem_lowpages(PAGE_SIZE);
        set_pmd(pmd, __pmd(__pa(pte) | _PAGE_TABLE));
        if (pte != pte_offset_kernel(pmd, 0)) {
            BUG();
        }
        return pte;
    }

    return pte_offset_kernel(pmd, 0);
}

/**
 * 896MB物理空间直接映射到内核线性空间
 */
static void __init kernel_physical_mapping_init(pgd_t *pgd_base) {
    pgd_t *pgd;
    pmd_t *pmd;
    pte_t *pte;
    int pgd_idx, pmd_idx, pte_idx;
    unsigned long pfn;
    unsigned long address;

    pgd_idx = pgd_index(PAGE_OFFSET);
    pgd = pgd_base + pgd_idx;
    pfn = 0;

    for (; pgd_idx < PTRS_PER_PGDIR; ++pgd, ++pgd_idx) {
        pmd = one_md_table_init(pgd);
        if (pfn >= max_low_pfn) {
            continue;
        }
        for (pmd_idx = 0; pmd_idx < PTRS_PER_PMD && pfn < max_low_pfn; ++pmd_idx, ++pmd) {
            pte = one_page_table_init(pmd);
            address = PAGE_OFFSET + pfn * PAGE_SIZE;
            for (pte_idx = 0; pte_idx < PTRS_PER_PTE && pfn < max_low_pfn; ++pte_idx, ++pte) {
                if (is_kernel_text(address)) {
                    set_pte(pte, pfn_pte(pfn, PAGE_KERNEL_EXEC));
                } else {
                    set_pte(pte, pfn_pte(pfn, PAGE_KERNEL));
                }
            }
        }
    }
}

void __init pagetable_init() {
    pgd_t *pgd_base = swapper_pg_dir;
    int i;

#ifdef CONFIG_X86_PAE
    for (i = 0; i < PTRS_PER_PGDIR; ++i) {
        set_pgd(pgd_base+i, __pgd(__pa(empty_zero_page) | _PAGE_PRESENT));
    }
#endif

    kernel_physical_mapping_init(pgd_base);
}
