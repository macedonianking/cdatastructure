#include "chapter7/quick_sort.h"

#include "util.h"

void quick_sort(int *buf, int n) {
    int i, j, t;
    if (!n || n <= 1) {
        return;
    }
    j = n - 1;
    i = 0;
    while (i < j) {
        if (buf[i] <= buf[j]) {
            ++i;
            continue;
        }
        t = buf[j];
        buf[j] = buf[i];
        buf[i] = buf[j-1];
        buf[j-1] = t;
        j--;
    }
    quick_sort(buf, j);
    quick_sort(buf+j+1, n-j);
}

void chapter7_17_problem() {
    int buf[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    quick_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}