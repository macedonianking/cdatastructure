#ifndef PRINTK_H
#define PRINTK_H

#include "linkage.h"

extern asmlinkage int printk(const char *format, ...) __attribute__((format(printf, 1, 2)));
extern void panic(const char *msg);

#endif
