#include "chapter7/algorithm7.h"

#include "macros.h"
#include "util.h"

static int partition(int *buf, int s, int e) {
    int r;
    int l;
    int j, t;

    r = e;
    l = s-1;
    for (j = s; j < r; ++j) {
        if (buf[j] <= buf[r]) {
            ++l;
            t = buf[l];
            buf[l] = buf[j];
            buf[j] = t;
        }
    }
    ++l;
    t = buf[l];
    buf[l] = buf[r];
    buf[r] = t;
    r = l;

    return r;   
}

static void quick_sort_recursive(int *buf, int s, int e) {
    int m;

    if (e <= s) {
        return;
    }
    m = partition(buf, s, e);
    quick_sort_recursive(buf, s, m-1);
    quick_sort_recursive(buf, m+1, e);
}

static void quick_sort(int *buf, int n) {
    if (n <= 1) {
        return;
    }
    quick_sort_recursive(buf, 0, n-1);
}

void algorithm_7_1_tutorial() {
    int buf[] = {13, 19, 9, 5, 12, 8, 7, 4, 21, 2, 6, 11};

    quick_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}