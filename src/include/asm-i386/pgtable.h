#ifndef _I386_PGTABLE_H
#define _I386_PGTABLE_H


#define KERNPG_TABLE    (_PAGE_PRESENT | _PAGE_RW | _PAGE_DIRTY | _PAGE_ACCESSED)

#define pmd_bad(pmd)        ((pmd_val(pmd) & (~PAGE_MASK & ~_PAGE_USER)) != KERNPG_TABLE)
// page存在_PAGE_PRESENT或者_PAGE_SIZE
#define pte_present(x)      ((x).pte_low & (_PAGE_PRESENT | _PAGE_PROTNONE))    

static inline int pte_user(pte_t pte) { return pte.pte_low & _PAGE_USER; }
static inline int pte_read(pte_t pte) { return pte.pte_low & _PAGE_USER; }
static inline int pte_write(pte_t pte) { return pte.pte_low & _PAGE_RW; }
static inline int pte_dirty(pte_t pte) { return pte.pte_low & _PAGE_DIRTY; }
static inline int pte_young(pte_t pte) { return pte.pte_low & _PAGE_ACCESSED; }

static inline pte_t pte_rdprotect(struct pte_t pte) { pte.pte_low &= ~_PAGE_USER; return pte; }
static inline pte_t pte_exprotect(struct pte_t pte) { pte.pte_low &= ~_PAGE_USER; return pte; }
static inline pte_t pte_mkclean(struct pte_t pte) { pte.pte_low &= ~_PAGE_DIRTY; return pte; }
static inline pte_t pte_mkold(struct pte_t pte) { pte.pte_low &= ~_PAGE_ACCESSED; return pte; }
static inline pte_t pte_wrprotect(struct pte_t pte) { pte.pte_low &= ~_PAGE_RW; return pte; }
static inline pte_t pte_mkread(struct pte_t pte) { pte.pte_low |= _PAGE_USER; return pte; }
static inline pte_t pte_mkexec(struct pte_t pte) { pte.pte_low |= _PAGE_USER; return pte; }
static inline pte_t pte_mkdirty(struct pte_t pte) { pte.pte_low |= _PAGE_DIRTY; return pte; }
static inline pte_t pte_mkyoung(struct pte_t pte) { pte.pte_low |= _PAGE_ACCESSED; return pte; }
static inline pte_t pte_mkwrite(struct pte_t pte) { pte.pte_low |= _PAGE_RW; return pte; }

#define pgd_index(addr)     (((addr) >> PGDIR_SHIFT) & (PTRS_PER_PGDIR - 1))
#define pgd_index_k(addr)   pgd_index(addr)

#endif // _I386_PGTABLE_H
