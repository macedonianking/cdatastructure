/*
 * entry.s
 *
 *  Created on: 2018年7月8日
 *      Author: lhb
 */

.section .text
.globl _do_divide_error
.globl _divide_error

.equ ORIG_EAX, 0x24
.equ ES,0x20
.equ __USER_DS, (15*8+3)

_divide_error:
	pushl $0
	pushl $_do_divide_error
	jmp common_error

common_error:
	pushl %ds
	pushl %eax
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	xorl %eax, %eax
	decl %eax
	movl ES(%esp), %edi
	movl ORIG_EAX(%esp), %edx
	movl %eax, ORIG_EAX(%esp)
	movl %es, %eax
	movl %eax, ES(%esp)
	movl $(__USER_DS), %eax
	movw %ax, %ds
	movw %ax, %es
	movl %esp, %eax
	pushl %edx
	pushl %eax
	call *%edi
	addl $8, %esp
	jmp ret_from_expception

.align
ret_from_expception:


