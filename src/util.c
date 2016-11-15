#include "util.h"

#include <stdlib.h>

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