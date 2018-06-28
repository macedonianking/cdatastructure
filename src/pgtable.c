/*
 * pgalloc.c
 *
 *  Created on: 2018年4月5日
 *      Author: lhb198710
 */

#include "config.h"

#include "pgalloc.h"
#include "slab.h"
#include "compiler.h"
#include "bug.h"
#include "fixmap.h"
#include "tlbflush.h"

struct kmem_cache_t *pgd_cache;
struct kmem_cache_t *pmd_cache;

static void set_pte_pfn(unsigned long vaddr, unsigned long pfn, pgprot_t flags) {
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    pgd = swapper_pg_dir + pgd_index(vaddr);
    if (pgd_none(*pgd)) {
        BUG();
        return;
    }

    pud = pud_offset(pgd, vaddr);
    if (pud_none(*pud)) {
        BUG();
        return;
    }

    pmd = pmd_offset(pud, vaddr);
    if (pmd_none(*pmd)) {
        BUG();
        return;
    }

    pte = pte_offset_kernel(pmd, vaddr);
    set_pte(pte, pfn_pte(pfn, flags)); //
    __flush_tlb_one(vaddr);
}

pgd_t *pgd_alloc(struct mm_struct *mm) {
    pgd_t *pgd;
    pmd_t *pmd;
    int i;

    pgd = (pgd_t*) kmem_cache_alloc(pgd_cache, GFP_KERNEL);
    if (!pgd || PTRS_PER_PMD == 1) {
        return pgd;
    }
    for (i = 0; i < USER_PTRS_PER_PGD; ++i) {
        pmd = (pmd_t*) kmem_cache_alloc(pmd_cache, GFP_KERNEL);
        if (!pmd) {
            goto out_oom;
        } //
        set_pgd(&pgd[i], __pgd(__pa(pmd)+1));
    }
    return pgd;

    out_oom: //
    for (i--; i >= 0; --i) {
        kmem_cache_free(pmd_cache, __va(pgd_val(pgd[i]) - 1));
    }
    kmem_cache_free(pgd_cache, pgd);
    return 0;
}

void pgd_free(pgd_t *pgd) {
    if (PTRS_PER_PMD > 1) {
        for (int i = 0; i < USER_PTRS_PER_PGD; ++i) {
            kmem_cache_free(pmd_cache, __va(pgd_val(pgd[i]) & PAGE_MASK));
        }
    }
    kmem_cache_free(pgd_cache, pgd);
}

pte_t *pte_alloc_one_kernel(struct mm_struct *mm, unsigned long addr) {
    return 0;
}

struct page *pte_alloc_one(struct mm_struct *mm, unsigned long addr) {
    return 0;
}

/**
 * Fixed map handing.
 */
void __set_fixmap(enum fixed_addresses idx, unsigned long phys, pgprot_t flags) {
    unsigned long vaddr = fix_to_virt(idx);
    if (idx >= __end_of_fixed_addresses) {
        BUG();
        return;
    }
    set_pte_pfn(vaddr, phys >> PAGE_SHIFT, flags);
}
