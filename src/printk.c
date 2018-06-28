/*
 * printk.c
 *
 *  Created on: 2018年4月14日
 *      Author: lhb198710
 */

#include "printk.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int asmlinkage printk(const char *format, ...) {
    va_list args;
    int n;

    va_start(args, format);
    n = vfprintf(stdout, format, args);
    va_end(args);
    return n;
}

void panic(const char *msg) {
    printf(msg);
}
