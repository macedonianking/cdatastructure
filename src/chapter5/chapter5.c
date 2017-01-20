#include "chapter5/chapter5.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "macros.h"
#include "util.h"

static int gHasInitializedRandomGenerator = 0;

static void checkInitializeRandomSeed() {
    if (!gHasInitializedRandomGenerator) {
        time_t seconds;
        time(&seconds);
        gHasInitializedRandomGenerator = 1;
        srand((unsigned int) seconds);
    }
}

/**
 * 获取最原始的随机量
 */
static int primitiveNextRandomBoolean() {
    checkInitializeRandomSeed();
    return rand() % 2;
}

void chapter5_main() {
    chapter5_1_2_problem();
}

/**
 * 获取随机数
 */
static int nextRandomInteger(int a, int b) {
    int n;
    int t;
    int k, bits;

    if (a > b) {
        t = a;
        a = b;
        b = t;
    }
    n = b - a + 1;
    bits = getIntegerBits((unsigned int)n);

    do {
        k = 0;
        for (int i = 0; i < bits; ++i) {
            k <<= 1;
            if (primitiveNextRandomBoolean()) {
                k |= 1;
            }
        }
    } while (k >= n);

    return a + k;
}

void chapter5_1_2_problem() {
    int n;
    for (int i = 0; i < 10; ++i) {
        n = nextRandomInteger(10, 20);
        fprintf(stdout, "%d\n", n);
    }
    fputc('\n', stdout);
}

static int biasedRandom(double factor) {
    int n;

    DCHECK(0 < factor && factor < 1.0);
    n = (int)(10000 * factor);
    return (rand() % 10000) <= n;
}

static int unbiasedRandom() {
    int a;
    int b;

    while (1) {
        a = biasedRandom(0.9);
        b = biasedRandom(0.9);
        if (a == 0 && b == 1) {
            return 0;
        } else if (a == 1 && b == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * 偏向随机
 */
void chapter5_1_3_problem() {
    for (int i = 0; i < 10; ++i) {
        fprintf(stdout, "%d\n", unbiasedRandom());
    }    
}
