/*
 * kernel.h
 *
 *  Created on: 2018年4月22日
 *      Author: lhb198710
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#define container_of(ptr, type, member) ({ \
    const typeof(((type*)(0))->member) *__mptr = (ptr); \
    (type*)((char*)__mptr - __builtin_offsetof(type, member)); \
})

extern int __kernel_text_address(unsigned long addr);

#endif /* KERNEL_H_ */
