#include "util.h"

#include <stdlib.h>

#include "log.h"
#include "macros.h"

void shuffle(int *data, int n) {
    int index, temp;

    while (--n > 0) {
        index = rand() % (n + 1);
        temp = *data;
        *data = *(data + index);
        *(data + index) = temp;
    }
}

int *linear_sequence(int start, int step, int n) {
    int *data = (int*) malloc(sizeof(int) * n);
    int *ptr = data;
    while (n-- > 0) {
        *ptr++ = start;
        start += step;
    }
    return data;
}

int next_capacity(int c) {
    int n = c << 1;
    int bits;

    DCHECK(c > 0);
    bits = 1;
    while (c > 0) {
        bits++;
        c >>= 1;
    }
    if (bits > 1 && (n & (1 << (bits - 1)))) {
        ++bits;
    }
    n = 1 << bits;
    DCHECK(n > 0);
    return n;
}

void dump_narray(int *buf, int n) {
    while (n-- > 0) {
        printf("%d ", *buf++);
    }
    printf("\n");
}

/**
 * 得到整数的位数
 */
int getIntegerBits(unsigned int n) {
    if (n == 0) {
        return 0;
    }

    int bits = 1;
    while (n >>= 1) {
        ++bits;   
    }
    return bits;
}
