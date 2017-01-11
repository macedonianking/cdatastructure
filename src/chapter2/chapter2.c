#include "chapter2/chapter2.h"

#include "macros.h"
#include "util.h"

static void insertion_sort(int *buf, int n) {
    int key;
    int i;

    DCHECK(n > 0);
    for (int j = 1; j < n; ++j) {
        key = buf[j];
        for (i = j-1; i >= 0 && buf[i] > key; --i) {
            buf[i+1] = buf[i];
        }
        buf[i+1] = key;
    }
}

static void insertion_sort_dsc(int *buf, int n) {
    DCHECK(n > 0);
    int key;
    int i;

    for (int j = 1; j < n; ++j) {
        key = buf[j];
        for (i = j-1; i >= 0 && buf[i] < key; --i) {
            buf[i+1] = buf[i];
        }
        buf[i+1] = key;
    }
}

void chapter2_1_1_problem() {
    int buf[] = {31, 41, 59, 26, 41, 58};
    insertion_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}

void chapter2_1_2_problem() {
    int buf[] = {31, 41, 59, 26, 41, 58};
    insertion_sort_dsc(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}

void chapter2_problem() {
    chapter2_3_1_tutorial();
}

static void selection_sort(int *buf, int n) {
    DCHECK(n > 0);
    int j;
    int t;

    for (int i = 0; i < n-1; ++i) {
        for (j = i+1; j<n; ++j) {
            if (buf[j] < buf[i]) {
                SWAP(buf[i], buf[j], t);
            }
        }
    }
}

/**
 * 选择排序的问题
 */
void chapter2_2_2_problem() {
    int buf[] = {31, 41, 59, 26, 41, 58};
    selection_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}

static void merge_impl(int *buf, int s, int m, int e, int *tmp) {
    DCHECK(s <= m && m < e);
    int i, j, k;
    int n1, n2;

    memcpy(tmp, buf+s, sizeof(int) * (e-s+1));
    n1 = m-s+1;
    n2 = e-s+1;
    i = 0;
    j = n1;
    k = s;
    while (i < n1 && j < n2) {
        if (tmp[i] <= tmp[j]) {
            buf[k++] = tmp[i++];
        } else {
            buf[k++] = tmp[j++];
        }
    }
    while (i < n1) {
        buf[k++] = tmp[i++];
    }
    while (j < n2)  {
        buf[k++] = tmp[j++];
    }
}

static void merge_sort_impl(int *buf, int s, int e, int *tmp) {
    int m;
    if (s < e) {
        m = (s+e) / 2;
        merge_sort_impl(buf, s, m, tmp);
        merge_sort_impl(buf, m+1, e, tmp);
        merge_impl(buf, s, m, e, tmp);
    }
}

static void merge_sort(int *buf, int n) {
    int *tmp = (int*) malloc(n * sizeof(int));
    DCHECK(tmp);
    merge_sort_impl(buf, 0, n-1, tmp);
    free(tmp);
}

void chapter2_3_1_tutorial() {
    int buf[] = {31, 41, 59, 26, 41, 58};
    merge_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}
