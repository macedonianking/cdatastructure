#ifndef _PGTABLE_2LEVEL_H
#define _PGTABLE_2LEVEL_H

#include <asm-generic/pgtable-nopmd.h>

#define pte_ERROR(x)    \
    printk("%s:%d bad pte %08lx.\n", __FILE__, __LINE__, (x).pte_low);
#define pgd_ERROR(x)    \
    printk("%s:%d bad pgd %08lx.\n", __FILE__, __LINE__, (x).pgd);

#define set_pte(pteptr, pteval) (*(pteptr) = (pteval))
#define set_pte_atomic(pteptr, pteval) set_pte(pteptr, pteval)
#define set_pmd(pmdptr, pmdval) (*(pmdptr) = (pmdval))

#define ptep_get_and_clear(xp) __pte(xchg(&(xp)->pte_low, 0))
#define pte_none(x)     (!(x).pte_low)
#define pte_same(a, b)  ((a).pte_low == (b).pte_low)
#define pte_page(x)     pfn_to_page(pte_pfn(x))
#define pte_pfn(x)      ((unsigned long)((x).pte_low >> PAGE_SHIFT))
#define pfn_to_pte(pfn, pgprot)   __pte(((pfn) << PAGE_SHIFT) | pgprot_val(pgprot))
#define pfn_to_pmd(pfn, pgprot)   __pmd(((pfn) << PAGE_SHIFT) | pgprot_val(pgprot))

#define pmd_page(x)     pfn_to_page(pmd_val(x) >> PAGE_SHIFT)
// 转为线性地址
#define pmd_page_kernel(x)          ((unsigned long ) __va(pmd_val(x) & PAGE_MASK))

#endif // _PGTABLE_2LEVEL_H
