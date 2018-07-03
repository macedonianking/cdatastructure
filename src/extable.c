/*
 * extable.c
 *
 *  Created on: 2018年7月2日
 *      Author: 80050136
 */

#include "kernel.h"

int __kernel_text_address(unsigned long addr) {
    return 1;
}
