#ifndef _PGTABLE_NOPUD_H
#define _PGTABLE_NOPUD_H

#ifndef __ASSEMBLY__

typedef struct { pgd_t pgd; } pud_t;

#define PUD_SHIFT       PGDIR_SHIFT
#define PTRS_PER_PUD    1
#define PUD_SIZE        (1UL << PUD_SHIFT)
#define PUD_MASK        (~(PUD_SIZE-1))

static inline int pgd_none(pgd_t pgd) { return 0; }
static inline int pgd_bad(pgd_t pgd) { return 0; }
static inline int pgd_present(pgd_t pgd) { return 1; }
static inline int pgd_clear(pgd_t *pgd) {  }

#define pud_val(x)  (pgd_val((x).pgd))
#define __pud(x)    ((pud_t) { __pgd(x) })

#endif // __ASSEMBLY__

#endif // _PGTABLE_NOPUD_H
