#include "chapter1/chapter1.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "macros.h"
#include "kdebug.h"

#ifndef KDEBUG

#define k1(n) (100 * (n) * (n))
#define k2(n) (pow(2, (n)))

#else

static int k1(int n) {
    return 100 * n * n;
}

static int k2(int n) {
    return pow(2, n);
}

#endif

void chapter1_2_3_problem() {
    int m;
    int n;
    int k;

    m = 1;
    DCHECK(k1(m) > k2(m));
    k = 1;
    n = m+1;
    while (k1(n) >= k2(n)) {
        m = n;
        n += k;
        k += 1;
    }
    
    k = (m + n + 1) / 2;
    while (n != k) {
        if (k1(k) >= k2(k)) {
            m = k;
        } else {
            n = k;
        }
        k = (m+n+1)/2;
    }
    fprintf(stdout, "n=%d\n", n);
}