/*
 * gfp.h
 *
 *  Created on: 2018年4月6日
 *      Author: lhb198710
 */

#ifndef GFP_H_
#define GFP_H_

#include "linkage.h"

#define GFP_KERNEL  0

struct page;

extern unsigned long FASTCALL(__get_free_pages(unsigned int gfp_mask, unsigned int order));
extern unsigned long FASTCALL(get_zeroed_page(unsigned int gfp_mask));
extern void FASTCALL(__free_pages(struct page*, int order));

#define __get_free_page(gfp_mask)   __get_free_pages(gfp_mask, 0)

#endif /* GFP_H_ */
