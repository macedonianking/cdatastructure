#ifndef _LINUX_BOOTMEM_H
#define _LINUX_BOOTMEM_H

extern unsigned long min_low_pfn;
extern unsigned long max_low_pfn;

void init_bootmem(unsigned long start, unsigned long end);

#endif
