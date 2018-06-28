/*
 * slab.h
 *
 *  Created on: 2018年4月5日
 *      Author: lhb198710
 */

#ifndef SLAB_H_
#define SLAB_H_

#include "gfp.h"

struct kmem_cache_t {
};

extern void *kmem_cache_alloc(struct kmem_cache_t *, int);
extern void kmem_cache_free(struct kmem_cache_t *, void *);

#endif /* SLAB_H_ */
