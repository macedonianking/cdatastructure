#include "chapter7/merge_sort.h"

#include "util.h"

void merge_sort(int *buf, int n) {
    int n1, n2;
    int i, j, k, t;
    n1 = n / 2;
    n2 = n - n1;
    if (n1 == 0 || n2 == 0) {
        return;
    }

    merge_sort(buf, n1);
    merge_sort(buf+n1, n2);
    i = 0;
    j = n1;
    while (i < j && j < n) {
        if (buf[i] <= buf[j]) {
            ++i;
            continue;
        }
        t = buf[j];
        for (k = j; k > i; --k) {
            buf[k] = buf[k-1];
        }
        buf[i] = t;
        ++j;
    }
}

void chapter7_13_problem() {
    int buf[] = {3, 1, 4, 1, 5, 9, 2, 6};
    merge_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}

static void merge_two_array(int *buf, int n1, int n2) {
    int i, j, k, t;
    if(!n1 || !n2) {
        return;
    }

    i = 0;
    j = n1;
    while (i < j && j < n1 + n2) {
        if(buf[i] <= buf[j]) {
            ++i;
            continue;
        }
        t = buf[j];
        for (k = j; k > i; --k) {
            buf[k] = buf[k-1];
        }
        buf[i] = t;
        ++j;
    }
}

static void chapter7_14_merge_sort(int *buf, int n) {
    int start, end;
    for (int step = 1; step < n; step <<=1) {
        start = 0;
        end = MIN(n, start + 2 * step);
        while (start < n) {
            merge_two_array(buf + start, step, end - start - step);
            start = end;
            end = MIN(n, start + 2 * step);
        }
    }
}

void chapter7_14_problem() {
    int buf[] = {3, 1, 4, 1, 5, 9, 2, 6};
    chapter7_14_merge_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}
