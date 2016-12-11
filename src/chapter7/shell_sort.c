#include "chapter7/shell_sort.h"

#include "util.h"
#include "macros.h"

void shell_sort(int *buf, int n) {
    int step, i, j;
    int temp;

    DCHECK(buf && n > 0);
    for (step = n / 2; step != 0; step >>= 1) {
        for (i = step; i < n; i += step) {
            temp = buf[i];
            for (j = i; j - step >= 0 && buf[j-step] > temp; j -= step) {
                buf[j] = buf[j-step];
            }
            buf[j] = temp;
        }
    }
}

void chapter7_4_problem_a() {
    int buf[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    shell_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}

static void shell_sort_by_step(int *buf, int n, int step) {
    int i, j;
    int temp;
    for (i = step; i < n; i++) {
        temp = buf[i];
        for (j = i; j - step >= 0 && buf[j-step] > temp; j -= step) {
            buf[j] = buf[j-step];
        }        
        buf[j] = temp;
    }
}

void chapter7_4_problem_b() {
    int buf[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    shell_sort_by_step(buf, NARRAY(buf), 7);
    shell_sort_by_step(buf, NARRAY(buf), 3);
    shell_sort_by_step(buf, NARRAY(buf), 1);
    dump_narray(buf, NARRAY(buf));
}