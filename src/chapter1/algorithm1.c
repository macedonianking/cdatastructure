#include "chapter1/algorithm1.h"

#include <stdio.h>
#include <math.h>

void algorithm1_2_2_problem() {
    int time1, time2;
    int n;

    n = 2;
    do {
        time1 = 8 * n * n;
        time2 = (int) 64 * n * log2(n);
        ++n;
    } while(time1 < time2);
    --n;
    fprintf(stdout, "n=%d\n", n);
}

void algorithm1_2_3_problem() {
    int time1, time2;
    int n;

    n = 1;
    do {
        time1 = 100 * n * n;
        time2 = pow(2, n);
        ++n;
    } while (time1 >= time2);
    --n;
    fprintf(stdout, "n=%d\n", n);
}