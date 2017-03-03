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
PAE开启的情况下27bits存储Page directory pointer table的地址
cr3寄存器被修改的是,Local TLB被自动清空
*/

/*
cr4寄存器
PSE扩展page开关
PGE避免经常被使用的page被交换到磁盘
PAE page address extension地址位数从32位扩展成34位
*/

/*
开启PAE
2-9-9-12的方式
entry是64bit位的
*/

#endif /* _ASM_I386_REGISTERS_H */
