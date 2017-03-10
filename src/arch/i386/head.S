.text

#include <asm/page.h>
#include <asm/segment.h>

ENTRY(startup_32)
    cld
    lgdt    boot_gdt_descr - __PAGE_OFFSET
    movl    $(__BOOT_DS), %eax
    movl    %ax, %ds
    movl    %ax, %es
    movl    %ax, %fs
    movl    %eax, %gs

.globl boot_gdt_descr

ALIGN
    .word   0
boot_gdt_descr:
    .word   
