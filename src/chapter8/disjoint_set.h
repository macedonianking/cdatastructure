#ifndef CPRIMER_CHAPTER8_DISJOINT_SET_H
#define CPRIMER_CHAPTER8_DISJOINT_SET_H

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "util.h"

struct disjoint_set {
    int     *queue;
    int     capacity;
};

struct union_pair {
    int k1;
    int k2;
};

// 初始化并查集
static void init_disjoint_set(struct disjoint_set *set, int capacity, int v) {
    DCHECK(capacity > 0);
    set->queue = (int*) malloc(sizeof(int) * capacity);
    set->capacity = capacity;
    for (int i = 0; i < capacity; ++i) {
        set->queue[i] = v;
    }
}

// 释放内存
static void free_disjoint_set(struct disjoint_set *set) {
    DCHECK(set->queue);
    free(set->queue);
    set->queue = NULL;
    set->capacity = 0;
}

// 查找所在的集合
static int disjoint_set_find(struct disjoint_set *set, int k1) {
    while (set->queue[k1] >= 0) {
        k1 = set->queue[k1];
    }
    return k1;
}

// dump结果
static void dump_disjoint_set(struct disjoint_set *set) {
    for (int i = 0; i < set->capacity; ++i) {
        if (set->queue[i] >= 0) {
            continue;
        }
        fprintf(stdout, "%2d : ", i);
        for (int j = 0; j < set->capacity; ++j) {
            if (disjoint_set_find(set, j) == i) {
                fprintf(stdout, "%d ", j);
            }
        }
        fprintf(stdout, "\n");
    }
}

#endif // CPRIMER_CHAPTER8_DISJOINT_SET_H