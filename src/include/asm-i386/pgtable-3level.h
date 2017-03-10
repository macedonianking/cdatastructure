#ifndef _PGTABLE_3LEVEL_H
#define _PGTABLE_3LEVEL_H

#include <asm-generic/pgtable-nopud.h>

#define pte_ERROR(x)
    printk("%s:%d bad pte %p(%08lx%08lx).\n", __FILE__, __LINE__, &(x), (x).pte_high, (x).pte_low);
#define pmd_ERROR(x)
    printk("%s:%d bad pmd %p(%016Lx).\n", __FILE__, __LINE__, &(x), pmd_val(x));
#define pgd_ERROR(x)
    printk("%s:%d bad pmd %p(%016Lx).\n", __FILE__, __LINE__, &(x), pgd_val(x));

#define pud_none(x)     0
#define pud_bad(x)      0
#define pud_present(x)  1

static inline void set_pte(pte_t *pteptr, pte_t pteval) {
    pteptr->pte_high = pteval.pte_high;
    smb_wmb();
    pteptr->pte_low = pteval.pte_low;
}

#define set_pte_atomic(pteptr, pteval) \
    set_64bit((unsigned long long *)(pteptr), pte_val(pteval))

#endif // _PGTABLE_3LEVEL_H
