#ifndef _ASM_I386_REGISTERS_H
#define _ASM_I386_REGISTERS_H

/*
cr0寄存器，控制寄存器
PG = 0, page功能开关
*/

// 控制page功能是否开启
#define CR0_PAGE_FLAGS  1

/*
cr2
save the linear address
when excepton 14 happened.
*/

/*
cr3
page directory physical address.
*/

/*
cr4寄存器
PSE扩展page开关
PGE避免经常被使用的page被交换到磁盘
*/

#endif /* _ASM_I386_REGISTERS_H */
