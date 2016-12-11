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
    if (n < 0) {
        fatal_error("next_capacity c two large");
    }
    bits = 0;
    while (c > 0) {
        bits++;
    }
    if (n & (1 << (bits - 1))) {
        bits++;
    }
    n = 1 << bits;
    if (n <= 0) {
        fatal_error("next_capacity two large");
    }
    return n;
}

void dump_narray(int *buf, int n) {
    while (n-- > 0) {
        printf("%d ", *buf++);
    }
    printf("\n");
}