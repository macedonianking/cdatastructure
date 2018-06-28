/*
 * count.c
 *
 *  Created on: 2018年4月26日
 *      Author: lhb198710
 */

#include "count.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <time.h>

#include "count_compiler.h"
#include "count_atomic.h"
#include "count_processor.h"

#define NLOOP   10000
#define NTHRAED  100

static void *thread_main_routing(void *);

struct thread_args {
    int nloop;
    int count;
};

struct thread_causal_args {
    int a;
    int b;
    int c;
};

void main_count() {
    thread_main_routing(NULL);
}

void *thread_main_routing(void *args) {
    return 0;
}
