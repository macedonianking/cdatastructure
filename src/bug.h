/*
 * bug.h
 *
 *  Created on: 2018年4月14日
 *      Author: lhb198710
 */

#ifndef BUG_H_
#define BUG_H_

#include "config.h"
#include "printk.h"

#define BUG() ; do {  \
    printk("kernel bug at %s:%d!\n", __FILE__, __LINE__); \
    panic("kernel bug!\n"); \
} while(0)

#define BUG_ON(condition)   ; do { if(condition) {BUG();} } while(0)

#endif /* BUG_H_ */
