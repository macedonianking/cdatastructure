/*
 * hash.h
 *
 *  Created on: 2018年4月30日
 *      Author: lhb198710
 */

#ifndef HASH_H_
#define HASH_H_

#define GOLDEN_RATIO_PRIME 0x9e370001UL

#define BITS_PER_LONG   (sizeof(long) << 3)

static inline unsigned long hash_long(unsigned long val, unsigned long bits) {
    unsigned long hash = val;

    hash *= GOLDEN_RATIO_PRIME;
    return hash >> (BITS_PER_LONG - bits);
}

#endif /* HASH_H_ */
