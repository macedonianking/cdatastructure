#ifndef _I386_PAGE_H
#define _I386_PAGE_H

#define PAGE_SHIFT      12
#define PAGE_SIZE       (1UL << PAGE_SHIFT)
#define PAGE_MASK       (~(PAGE_SIZE-1))

#define LARGE_PAGE_MASK     (LARGE_PAGE_SIZE-1)
#define LARGE_PAGE_SIZE     (1UL << PMD_SHIFT)

#ifndef __ASSEMBLY__

// Page address extension开启
#ifdef CONFIG_X86_PAE

typedef struct { unsigned long pte_low, pte_high; } pte_t;
typedef struct { unsigned long long pmd; } pmd_t;
typedef struct { unsigned long long pgd; } pgd_t;
typedef struct { unsigned long long pgprot; } pgprot_t;

#define pmd_val(x)  ((x).pmd)
#define pte_val(x)  (x).pte_low | ((unsigned long long) (x).pte_high << 32)
#define __pmd(x) ((pmd_t) {(x)})
#else
typedef struct { unsigned long pte_low; } pte_t;
typedef struct { unsigned long pgd; } pgd_t;
typedef struct { unsigned long pgprot; } pgprot_t;
#define pte_val(x) ((x).pte_low)
#endif // CONFIG_X86_PAE

#define pgd_val(x)      ((x).pgd)
#define pgprot_val(x)   ((x).pgprot)

#define __pte(x)    ((pte_t) { (x) })
#define __pgd(x)    ((pgd_t) { (x) })

#endif // __ASSEMBLY__

#endif // _I386_PAGE_H
