#include "chapter2/algorithm2_3.h"

#include "macros.h"
#include "util.h"

static void merge_sort_impl(int *buf, int m, int n, int *tmp) {
    DCHECK(m <= n);
    int i, j, v;
    int k;
    if (m == n) {
        return;
    }
    k = (m + n) / 2;
    merge_sort_impl(buf, m, k, tmp);
    merge_sort_impl(buf, k+1, n, tmp);

    i = m;
    j = k+1;
    v = 0;
    while (i <= k && j <= n) {
        if (buf[i] < buf[j]) {
            tmp[v++] = buf[i++];
        } else {
            tmp[v++] = buf[j++];
        }
    }
    while (i <= k) {
        tmp[v++] = buf[i++];
    }
    while (j <= n) {
        tmp[v++] = buf[j++];
    }
    v = 0;
    for (i = m; i <= n; ++i) {
        buf[i] = tmp[v++];
    }
}

static void merge_sort(int *buf, int n) {
    DCHECK(n > 0);
    int *tmp = (int*) malloc(sizeof(int) * n);
    DCHECK(tmp);
    merge_sort_impl(buf, 0, n-1, tmp);
    free(tmp);
}


void algorithm_2_3_1_tutorial() {
    int buf[] = {5, 2, 4, 7, 1, 3, 2, 6};
    merge_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}

void algorithm_2_3_1_problem() {
    int buf[] = {3, 41, 52, 26, 38, 57, 8, 49};
    merge_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}

static int binary_search(int *buf, int s, int e, int k) {
    int m;
    if (e < s) {
        return -1;
    }
    
    m = (s + e) / 2;
    if (buf[m] == k) {
        return m;
    } else if (buf[m] < k) {
        return binary_search(buf, m+1, e, k);
    } else {
        return binary_search(buf, s, m-1, k);
    }
}

void algorithm_2_3_5_problem() {
    int buf[] = {3, 41, 52, 26, 38, 57, 8, 49};
    int k;
    merge_sort(buf, NARRAY(buf));
    k = binary_search(buf, 0, NARRAY(buf)-1, 49);
    fprintf(stdout, "%d\n", k);
}

static void algorithm_2_3_7_problem_solution(int *a, int *b, int *buf, int n, int total) {
    *a = *b = -1;
    int k;
    for (int i = 0; i < n; i++) {
        k = binary_search(buf, 0, n-1, total - buf[i]);
        if (k != -1 && k != i) {
            *a = i;
            *b = k;
            break;
        }
    }
}

void algorithm_2_3_7_problem() {
    int buf[] = {3, 41, 52, 26, 38, 57, 8, 49};
    int a, b;
    merge_sort(buf, NARRAY(buf));
    algorithm_2_3_7_problem_solution(&a, &b, buf, NARRAY(buf), 44);
    if (a != -1 && b != -1){
        fprintf(stdout, "%d + %d = %d\n", buf[a], buf[b], 44);
    }
}