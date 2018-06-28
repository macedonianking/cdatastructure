/*
 * pgtable.h
 *
 *  Created on: 2018年4月1日
 *      Author: lhb198710
 */

#ifndef PGTABLE_H_
#define PGTABLE_H_

#include "config.h"
#include "bitops.h"

#define _PAGE_BIT_PRESENT           0
#define _PAGE_BIT_RW                1
#define _PAGE_BIT_USER              2
#define _PAGE_BIT_PWT               3
#define _PAGE_BIT_PCD               4
#define _PAGE_BIT_ACCESSED          5
#define _PAGE_BIT_DIRTY             6
#define _PAGE_BIT_PSE               7
#define _PAGE_BIT_NX                63

#define _PAGE_PRESENT               0x0001
#define _PAGE_RW                    0x0002
#define _PAGE_USER                  0x0004
#define _PAGE_PWT                   0x0008
#define _PAGE_PCD                   0x0010
#define _PAGE_ACCESSED              0x0020
#define _PAGE_DIRTY                 0x0040
#define _PAGE_PSE                   0x0080

#ifdef CONFIG_PAE_X86
#define _PAGE_NX                    (1ULL << _PAGE_BIT_NX)
#else
#define _PAGE_NX                    0
#endif

#define _PAGE_FILE                  0x040
#define _PAGE_PROTNONE              0x080
#define _PAGE_CHG_MASK              (PTE_MASK | _PAGE_ACCESSED | _PAGE_DIRTY)

#define _KERNPG_TABLE   (_PAGE_PRESENT | _PAGE_RW | _PAGE_ACCESSED | _PAGE_DIRTY)
#define _PAGE_KERNEL    (_PAGE_PRESENT | _PAGE_RW | _PAGE_ACCESSED | _PAGE_DIRTY | _PAGE_NX)
#define _PAGE_KERNEL_EXEC   (_PAGE_PRESENT | _PAGE_RW | _PAGE_ACCESSED | _PAGE_DIRTY)
#define _PAGE_KERNEL_NOCACHE    (_PAGE_KERNEL | _PAGE_PCD)
#define _PAGE_TABLE     (_PAGE_PRESENT | _PAGE_RW | _PAGE_ACCESSED | _PAGE_DIRTY | _PAGE_USER)

/**
 * 设置在PTE ENTRY
 */
#define PAGE_KERNEL         __pgprot(_PAGE_KERNEL)
#define PAGE_KERNEL_EXEC    __pgprot(_PAGE_KERNEL_EXEC)
#define PAGE_KERNEL_NOCACHE __pgprot(_PAGE_KERNEL_NOCACHE)

#include "page.h"

#ifdef CONFIG_X86_PAE
#include "pgtable-3levels.h"
#else
#include "pgtable-2levels.h"
#endif

#define PUD_SHIFT       PGDIR_SHIFT
#define PTRS_PER_PUD    1

#ifndef CONFIG_X86_PAE
#define PMD_SHIFT       PUD_SHIFT
#define PTRS_PER_PMD    1
#endif

#define PGDIR_SIZE  (1UL << PGDIR_SHIFT)
#define PGDIR_MASK  (~(PGDIR_SIZE-1))

#define PUD_SIZE    (1UL << PUD_SHIFT)
#define PUD_MASK    (~(PUD_SIZE-1))

#define PMD_SIZE    (1UL << PMD_SHIFT)
#define PMD_MASK    (~(PMD_SIZE-1))

#define pte_clear(pteptr)   set_pte(pteptr, __pte(0))
#define pte_present(pte)    (((pte).pte_low) & (_PAGE_PRESENT | _PAGE_PROTNONE))

#define pte_user(pte)       (((pte).pte_low) & _PAEG_USER)
#define pte_read(pte)       (((pte).pte_low) & _PAGE_USER)
#define pte_write(pte)      (((pte).pte_low) & _PAGE_RW)
#define pte_exec(pte)       (((pte).pte_low) & _PAGE_USER)
#define pte_dirty(pte)      (((pte).pte_low) & _PAGE_DIRTY)
#define pte_young(pte)      (((pte).pte_low) & _PAGE_ACCESSED)
#define pte_file(pte)       (((pte).pte_low) & _PAGE_FILE)

#define mk_pte_huge(pte)    ((pte).pte_low |= _PAGE_PRESENT | _PAGE_PSE)
#define pte_wrprotect(pte)  ((pte).pte_low &= ~_PAGE_RW)
#define pte_rdprotect(pte)  ((pte).pte_low &= ~_PAGE_USER)
#define pte_exprotect(pte)  ((pte).pte_low &= ~_PAGE_USER)
#define pte_mkwrite(pte)    ((pte).pte_low |= _PAGE_RW)
#define pte_mkread(pte)     ((pte).pte_low |= _PAGE_USER)
#define pte_mkexec(pte)     ((pte).pte_low |= _PAGE_USER)
#define pte_mkclean(pte)    ((pte).pte_low &= ~_PAGE_DIRTY)
#define pte_mkdirty(pte)    ((pte).pte_low |= _PAGE_DIRTY)
#define pte_mkyoung(pte)    ((pte).pte_low |= _PAGE_ACCESSED)
#define pte_mkold(pte)      ((pte).pte_low &= ~_PAGE_ACCESSED)

static inline pte_t pte_modify(pte_t pte, pgprot_t prot) {
    pte.pte_low &= _PAGE_CHG_MASK;
    pte.pte_low |= pgprot_val(prot);
    return pte;
}

static inline void ptep_set_wrprotect(pte_t *ptep) {
    clear_bit(_PAGE_BIT_RW, &ptep->pte_low);
}

static inline void ptep_mkdirty(pte_t *ptep) {
    set_bit(_PAGE_BIT_DIRTY, &ptep->pte_low);
}

static inline int ptep_test_and_clear_dirty(pte_t *ptep) {
    if (!pte_dirty(*ptep)) {
        return 0;
    }
    return test_and_clear_bit(_PAGE_BIT_DIRTY, &ptep->pte_low);
}

static inline int ptep_test_and_clear_young(pte_t *ptep) {
    if (!pte_young(*ptep)) {
        return 0;
    }
    return test_and_clear_bit(_PAGE_BIT_ACCESSED, &ptep->pte_low);
}

#define __HAVE_ARCH_PTEP_GET_AND_CLEAR

#define set_pgd(pgdptr, pgdval) set_pud((pud_t*)(pgdptr), (pud_t){(pgdval)})

#define pmd_large(pmd)  ((pmd_val(pmd) & (_PAGE_PRESENT | _PAGE_PSE)) == (_PAGE_PRESENT | _PAGE_PSE))
#define pmd_none(pmd)   (!pmd_val(pmd))
#define pmd_present(pmd)    (pmd_val(pmd) & _PAGE_PRESENT)
#define pmd_clear(pmdptr)   ; do {set_pmd(pmdptr, __pmd(0)); } while(0)
#define pmd_bad(pmd)    ((pmd_val(pmd) & (~PAGE_MASK & ~_PAGE_USER)) != _KERNPG_TABLE)

static inline void pgd_clear(pgd_t *pgdptr) {
}

static inline int pgd_none(pgd_t pgd) {
    return 0;
}
#define pgd_bad(pgd)        0
#define pgd_present(pgd)    1

#define pud_clear(pudptr)   set_pud(pudptr, __pud(0))
#define pud_none(pud)       0
#define pud_bad(pud)        0
#define pud_present(pud)    1

#define pgd_index(addr)         (((addr) >> PGDIR_SHIFT) & (PTRS_PER_PGDIR-1))
#define pgd_index_k(addr)       pgd_index(addr)

#define pgd_offset(mm, addr)    ((mm)->pgd + pgd_index(addr))
#define pgd_offset_k(addr)      pgd_offset(&init_mm, addr)

#define pgd_page(pgd)           pud_page((pud_t){(pgd)})
#define pgd_page_kernel(pgd)    pud_page((pud_t){(pgd)})

static inline pud_t *pud_offset(pgd_t *pgd, unsigned long addr) {
    return (pud_t*) pgd;
}

#define pmd_index(addr)             (((addr) >> PMD_SHIFT) & PTRS_PER_PMD)
#define pmd_page(pmd)               (pfn_to_page((pmd_val(pmd) >> PAGE_SHIFT)))
/**
 * 获取的是pmd对应的页面的linear address
 */
#define pmd_page_kernel(pmd)        ((struct page*)__va(pmd_val(pmd) & PAGE_MASK))

#define mk_pte(page, pgprot)        pfn_pte(page_to_pfn(page), pgprot)

#define pte_index(addr)                 (((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE))
#define pte_offset_kernel(dir, addr)    ((pte_t*)pmd_page_kernel(*(dir)) + pte_index(addr))

#define pte_page(pte)           pfn_to_page(pte_pfn(pte))

#define pfn_pmd(pfn, pgprot)    __pmd(((pfn) << PAGE_SHIFT) | pgprot_val(pgprot))

#define TASK_SIZE   PAGE_OFFSET
#define USER_PTRS_PER_PGD   (TASK_SIZE/PGDIR_SIZE)

#define pud_free(pud)           do {} while(0)
#define pmd_free(pmd)           do {} while(0)

struct page;
struct mm_struct;
struct kmem_cache_t;

extern unsigned long empty_zero_page[1024];
extern struct kmem_cache_t *pgd_cache;
extern struct kmem_cache_t *pmd_cache;
extern pgd_t swapper_pg_dir[];
extern pte_t pg0[];

extern struct page *pte_alloc_one(struct mm_struct*, unsigned long);
extern pte_t *pte_alloc_one_kernel(struct mm_struct *, unsigned long);

#endif /* PGTABLE_H_ */
