/*
 * module.h
 *
 *  Created on: 2018年3月19日
 *      Author: lhb198710
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "config.h"

#define MODULE_SYMBOL_PREFIX ""

struct kernel_symbol {
    unsigned long value;
    const char *name;
};

#ifdef CONFIG_MODULES

#ifndef __GENSYMS__

#ifdef CONFIG_MODVERSIONS
#define __CRC_SYMBOL(sym, sec)  \
    extern void *__crc_##sym __attribute__((weak));  \
    static const unsigned long __kcrctab_##sym \
    __attribute__((section("__kcrctab" sec), unused)) = (unsigned long) &__crc_##sym;
#else
#define __CRC_SYMBOL(sym, sec)
#endif

#define __EXPORT_SYMBOL(sym, sec)   \
    __CRC_SYMBOL(sym, sec); \
    static const char __ksymstr_##sym[] \
    __attribute__((section("__ksymtab_strings"))) \
    = MODULE_SYMBOL_PREFIX #sym;    \
    static const struct kernel_symbol __ksymtab_##sym \
    __attribute__((section("__ksymtab" sec), unused)) \
    = { (unsigned long) &sym, __ksymstr_##sym }

#define EXPORT_SYMBOL(sym)      __EXPORT_SYMBOL(sym, "")
#define EXPORT_SYMBOL_GPL(sym)  __EXPORT_SYMBOL(sym, "_gpl")

#endif /*__GENSYMS__*/

#else /*CONFIG_MODULES*/

#define EXPORT_SYMBOL(sym)
#define EXPORT_SYMBOL_GPL(sym)

#endif /*CONFIG_MODULES*/

#endif /* MODULE_H_ */
