#include <linux/bootmem.h>

/*
_text, _etext, 代码取
_edata, 初始化数据区
_end 未初始化数据区
*/

unsigned long min_low_pfn;
unsigned long max_low_pfn;
unsigned long max_pfn;

void init_bootmem(unsigned long start, unsigned long pages) {
     min_low_pfn = start;
     max_low_pfn = pages;
}
