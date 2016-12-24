#include "chapter2/algorithm2_1.h"

#include "macros.h"
#include "util.h"

static void insertion_sort_asc(int *buf, int n) {
    DCHECK(n > 0);
    int j;
    int t;
    
    for (int i = 1; i < n; ++i) {
        t = buf[i];
        for (j = i; j > 0 && buf[j-1] > t; --j) {
            buf[j] = buf[j-1];
        }
        buf[j] = t;
    }
}

static void insertion_sort_dsc(int *buf, int n) {
   DCHECK(n > 0);
   int j, t;

   for (int i = 1; i < n; ++i) {
       t = buf[i];
       for (j = i; j > 0 && buf[j-1] < t; --j) {
           buf[j] = buf[j-1];
       }
       buf[j] = t;
   }
}

void algorithm_2_1_1_problem() {
    int data[] = { 31, 41, 59, 26, 41, 58, };
    insertion_sort_asc(data, NARRAY(data));
    dump_narray(data, NARRAY(data));
}

void algorithm_2_1_2_problem() {
    int data[] = { 31, 41, 59, 26, 41, 58, };
    insertion_sort_dsc(data, NARRAY(data));
    dump_narray(data, NARRAY(data));
}
