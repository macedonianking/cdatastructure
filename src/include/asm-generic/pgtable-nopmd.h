#ifndef _PGTABLE_NOPMD_H
#define _PGTABLE_NOPMD_H

#ifndef __ASSEMBLY__

#include <asm-generic/pgtable-nopud.h>

typedef struct { pud_t pud; } pmd_t;

#define PMD_SHIFT       PUD_SHIFT
#define PTRS_PER_PMD    1
#define PMD_SIZE        (1UL << PUD_SHIFT)
#define PMD_MASK        (~(PMD_SIZE-1))

#define pmd_val     (pud_val((x).pud))
#define __pmd       ((pmd_t) { __pmd(x) })

#endif // __ASSEMBLY__

#endif // _PGTABLE_NOPMD_H
