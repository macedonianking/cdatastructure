#include "chapter7/insertion_sort.h"

#include "macros.h"
#include "util.h"

void insertion_sort(int *buf, int n) {
    int tmp;
    int i, j;
    DCHECK(buf && n > 0);
    for (i = 1; i < n; ++i) {
        tmp = buf[i];
        for (j = i; j > 0 && buf[j - 1] > tmp; --j) {
            buf[j] = buf[j-1];
        }
        buf[j] = tmp;
    }
}

void chapter7_1_problem() {
    int buf[] = {3, 1, 4, 1, 5, 9, 6, 2, 5};
    insertion_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}