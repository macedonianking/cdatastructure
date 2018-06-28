/*
 * compiler.h
 *
 *  Created on: 2018年3月6日
 *      Author: lhb198710
 */

#ifndef COMPILER_H_
#define COMPILER_H_

#include "config.h"

#define barrier()   asm volatile("":::"memory")

#define RELOC_HIDE(ptr, offset)  \
({                              \
    unsigned long __ptr;        \
    asm volatile("":"=g"(__ptr):"0"(ptr)); \
    (typeof(ptr))(__ptr + (offset));    \
})

#define __always_inline __attribute__((always_inline))
#ifndef __always_inline
#define __always_inline inline
#endif

#define __unused __attribute__((unused))

#endif /* COMPILER_H_ */
