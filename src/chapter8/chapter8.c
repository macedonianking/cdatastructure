#include "chapter8/chapter8.h"

#include "chapter8/disjoint_set.h"

// k2代表的子树并入到k1的集合中
static void disjoint_set_union(struct disjoint_set *set, int k1, int k2) {
    set->queue[k2] = k1;
}

void chapter8_1_problem_a() {
    struct disjoint_set set;
    struct union_pair pairs[] = {
        {1, 2},
        {3, 4},
        {3, 5},
        {1, 7},
        {3, 6},
        {8, 9},
        {3, 10},
        {3, 11},
        {3, 12},
        {3, 13},
        {14, 15},
        {16, 17},
        {14, 16},
        {1, 3},
        {1, 14},
    };
    struct union_pair *ptr;
    
    init_disjoint_set(&set, 18, -1);
    for (int i = 0; i < NARRAY(pairs); ++i) {
        ptr = pairs + i;
        disjoint_set_union(&set, ptr->k1, ptr->k2);
    }
    fputs("chapter8_1_problem_a:\n", stdout);
    dump_disjoint_set(&set);
    free_disjoint_set(&set);
}