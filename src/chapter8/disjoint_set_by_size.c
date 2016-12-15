#include "chapter8/chapter8.h"

#include "chapter8/disjoint_set.h"

// 并查集集合合并
static void disjoint_set_union(struct disjoint_set *set, int k1, int k2) {
    k1 = disjoint_set_find(set, k1);
    k2 = disjoint_set_find(set, k2);
    if (set->queue[k2] < set->queue[k1]) {
        int t = k1;
        k1 = k2;
        k2 = t;
    }
    if (k1 != k2) {
        set->queue[k1] += set->queue[k2];
        set->queue[k2] = k1;
    }
}

static int find_deep(struct disjoint_set *set, int k1) {
    if (set->queue[k1] < 0) {
        return 0;
    }
    return find_deep(set, set->queue[k1]) + 1;
}

static int find_deepest_node(struct disjoint_set *set) {
    int max_index = -1;
    int max_deep = -1;
    int deep;
    
    for (int i = 0; i < set->capacity; ++i) {
        deep = find_deep(set, i);
        if (deep > max_deep) {
            max_index = i;
            max_deep = deep;
        }
    }
    return max_index;
}

static int path_compression(struct disjoint_set *set, int k1) {
    if (set->queue[k1] < 0) {
        return k1;
    }
    set->queue[k1] = path_compression(set, set->queue[k1]);
    return set->queue[k1];
}

void chapter8_1_problem_b() {
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
    fputs("chapter8_1_problem_b:\n", stdout);
    dump_disjoint_set(&set);
    free_disjoint_set(&set);
}

void chapter8_2_problem_c() {
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
    int max_index;
    
    init_disjoint_set(&set, 18, -1);
    for (int i = 0; i < NARRAY(pairs); ++i) {
        ptr = pairs + i;
        disjoint_set_union(&set, ptr->k1, ptr->k2);
    }
    max_index = find_deepest_node(&set);
    DCHECK(max_index != -1);
    path_compression(&set, max_index);
    free_disjoint_set(&set);
}

void chapter8_6_problem() {
    struct disjoint_set set;
    struct union_pair pairs[] = {
        {1, 2},
        {3, 4},
        {3, 6},
        {5, 7},
        {4, 6},
        {2, 4},
        {8, 9},
        {5, 8},
    };
    struct union_pair *ptr;

    init_disjoint_set(&set, 10, -1);
    for (int i = 0; i < NARRAY(pairs); ++i) {
        ptr = pairs + i;
        disjoint_set_union(&set, ptr->k1, ptr->k2);
    }
    dump_disjoint_set(&set);
    free_disjoint_set(&set);
}