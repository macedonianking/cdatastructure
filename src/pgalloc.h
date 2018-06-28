/*
 * pgallow.h
 *
 *  Created on: 2018年4月5日
 *      Author: lhb198710
 */

#ifndef PGALLOC_H_
#define PGALLOC_H_

#include "sched.h"
#include "page.h"
#include "pgtable.h"

extern pgd_t *pgd_alloc(struct mm_struct *mm);
extern void pgd_free(pgd_t *pgd);

#endif /* PGALLOC_H_ */
