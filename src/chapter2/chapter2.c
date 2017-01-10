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
    chapter2_1_2_problem();
}
