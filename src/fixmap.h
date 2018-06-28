/*
 * fixmap.h
 *
 *  Created on: 2018年4月15日
 *      Author: lhb198710
 */

#ifndef FIXMAP_H_
#define FIXMAP_H_

#include "config.h"
#include "compiler.h"

enum fixed_addresses {
    FIX_HOLE, //
    FIX_VSYSCALL, // 
    __end_of_permanent_fixed_address,
#define NR_FIX_BTMAPS   16
    FIX_BTMAP_END = __end_of_permanent_fixed_address, //
    FIX_BTMAP_START = FIX_BTMAP_END + NR_FIX_BTMAPS - 1, //
    FIX_WP_TEST, //
    __end_of_fixed_addresses, //
};

#define __FIXADDR_TOP   0xfffff000

#define FIXADDR_TOP             ((unsigned long) __FIXADDR_TOP)
#define __FIXADDR_SIZE          (__end_of_permanent_fixed_address << PAGE_SHIFT)
#define __FIXADDR_BOOT_SIZE     (__end_of_fixed_addresses << PAGE_SHIFT)
#define FIXADDR_START           (FIXADDR_TOP - __FIXADDR_SIZE)
#define FIXADDR_BOOT_START      (FIXADDR_TOP - __FIXADDR_BOOT_SIZE)

#define __fix_to_virt(index)    (FIXADDR_TOP - ((index) << PAGE_SHIFT))
#define __virt_to_fix(addr)     ((FIXADDR_TOP - ((addr) & PAGE_MASK)) >> PAGE_SHIFT)

extern void __this_fixmap_does_not_exist();
extern void __set_fixmap(enum fixed_addresses idx, unsigned long phy, pgprot_t pgprot);

#define set_fixmap(idx, phy)            __set_fixmap(idx, phy, PAGE_KERNEL)
#define set_fixmap_nocache(idx, phy)    __set_fixmap(idx, phy, PAGE_KERNEL_NOCACHE)
#define clear_fixmap(idx)               __set_fixmap(idx, 0, __pgprot(0))

static inline unsigned long fix_to_virt(int index) {
    if (index >= __end_of_fixed_addresses) {
        BUG();
    }
    return __fix_to_virt(index);
}

static inline int virt_to_fix(unsigned long addr) {
    BUG_ON(!(FIXADDR_START <= addr && addr < FIXADDR_TOP));
    return __virt_to_fix(addr);
}

#endif /* FIXMAP_H_ */
