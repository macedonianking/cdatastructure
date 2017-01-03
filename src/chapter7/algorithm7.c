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

static int partition_myself(int *buf, int s, int e) {
    int t;
    DCHECK(s <= e);

    while (s < e) {
        if (buf[s] <= buf[e]) {
            ++s;
        } else {
            t = buf[s];
            buf[s] = buf[e-1];
            buf[e-1] = buf[e];
            buf[e] = t;
            --e;  
        }
    }
    return e;
}

static void quick_sort_recursive_myself(int *buf, int s, int e) {
    int m;
    if (s <= e) {
        m = partition_myself(buf, s, e);
        quick_sort_recursive_myself(buf, s, m-1);
        quick_sort_recursive_myself(buf, m+1, e);
    }
}

static void quick_sort_myself(int *buf, int n) {
    quick_sort_recursive_myself(buf, 0, n-1);
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

static void algorithm_7_1_tutorial_a() {
    int buf[] = {13, 19, 9, 5, 12, 8, 7, 4, 21, 2, 6, 11};

    quick_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}

static void algorithm_7_1_tutorial_b() {
    int buf[] = {13, 19, 9, 5, 12, 8, 7, 4, 21, 2, 6, 11};

    quick_sort_myself(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}

void algorithm_7_1_tutorial() {
    algorithm_7_1_tutorial_a();
    algorithm_7_1_tutorial_b();
}

static int parititin_7_1_2(int *buf, int s, int e) {
    int i;
    int use_less;
    int swap;
    int t;
    
    DCHECK(s <= e);
    i = s-1;
    use_less = 1;
    while (s < e) {
        swap = use_less ? buf[s] <= buf[e] : buf[s] < buf[e];
        if (swap) {
            use_less = !use_less;
            ++i;
            t = buf[i];
            buf[i] = buf[s];
            buf[s] = t; 
        }
        ++s;
    }
    ++i;
    t = buf[i];
    buf[i] = buf[e];
    buf[e] = t;
    return i;
}

static void quick_sort_7_1_2(int *buf, int s, int e) {
    int m;
    if (s <= e) {
        m = parititin_7_1_2(buf, s, e);
        quick_sort_7_1_2(buf, s, m-1);
        quick_sort_7_1_2(buf, m+1, e);
    }
}

void algorithm_7_1_2_problem() {
    int buf[] = {13, 19, 9, 5, 12, 8, 7, 4, 21, 2, 6, 11};
    quick_sort_7_1_2(buf, 0, NARRAY(buf)-1);
    dump_narray(buf, NARRAY(buf));
}

static int partition_7_1_4(int *buf, int s, int e) {
    int i;
    int t;

    i = s-1;
    while (s < e) {
        if (buf[s] >= buf[e]) {
            ++i;
            t = buf[i];
            buf[i] = buf[s];
            buf[s] = t;
        }
        ++s;
    }
    ++i;
    t = buf[i];
    buf[i] = buf[e];
    buf[e] = t; 
    return i;   
}

/**
 * 逆序排序
 */
static void quick_sort_7_1_4(int *buf, int s, int e) {
    int m;

    if (s < e) {
        m = partition_7_1_4(buf, s, e);
        quick_sort_7_1_4(buf, s, m-1);
        quick_sort_7_1_4(buf, m+1, e);
    }
}

void algorithm_7_1_4_problem() {
    int buf[] = {13, 19, 9, 5, 12, 8, 7, 4, 21, 2, 6, 11};
    quick_sort_7_1_4(buf, 0, NARRAY(buf)-1);
    dump_narray(buf, NARRAY(buf));
}
