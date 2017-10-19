#include "chapter6/heap_sort.h"

struct heap_entity {
    int *ptr;
    int n;
};

#define PARENT(x)   ((x-1) >> 1)
#define LEFT(x)     ((x) * 2 + 1)
#define RIGHT(x)    ((x) * 2 + 2)

void heap_sort_main(int argc, char **argv) {
}

/**
 * 维护二叉堆的性质
 */
void max_heapify(struct heap_entity *ptr, int i) {
    int key;
    int max, left, right;

    if (!(0 <=i && i < ptr->n)) {
        return;
    }

    key = ptr->ptr[i];
    for (;;) {
        max = i;
        left = LEFT(i);
        right = RIGHT(i);
        if (left < ptr->n && ptr->ptr[left] > key) {
            max = left;
        }
        if (right < ptr->n && ptr->ptr[right] < key) {
            max = right;
        }
        if (max != i) {
            ptr->ptr[i] = ptr->ptr[max];
            i = max;
        } else {
            break;
        }
    }
    ptr->ptr[i] = key;
}

/**
 * 向上维护二叉堆的性质
 */
void max_heapify_percolate_up(struct heap_entity *ptr, int i) {
    int key;
    int p;

    if (!(0 < i && i < ptr->n)) {
        return;
    }

    key = ptr->ptr[i];
    while (i > 0) {
        p = PARENT(i);
        if (key > ptr->ptr[p]) {
            ptr->ptr[i] = ptr->ptr[p];
            i = p;
        } else {
            break;
        }
    }
    ptr->ptr[i] = key;
}

/**
 * 二叉堆
 */
void max_heapify_build(struct heap_entity *ptr) {
    if (!ptr->n) {
        return;
    }

    for (int i = PARENT(ptr->n - 1) - 1; i >= 0; --i) {
        max_heapify(ptr, i);
    }
}

void max_heapify_sort(int *buf, int n) {
    struct heap_entity entity, *ptr;
    int t;

    if (!n) {
        return;
    }

    ptr = &entity;
    ptr->ptr = buf;
    ptr->n = n;
    max_heapify_build(ptr);

    while (ptr->n > 1) {
        t = ptr->ptr[0];
        ptr->ptr[0] = ptr->ptr[ptr->n - 1];
        ptr->ptr[ptr->n - 1] = t;
        --ptr->n;
        max_heapify(ptr, 0);
    }
}

int max_heapify_pop_first(struct heap_entity *ptr) {
    int r = ptr->ptr[0];
    ptr->ptr[0] = ptr->ptr[ptr->n - 1];
    --ptr->n;
    if (ptr->n > 0) {
        max_heapify(ptr, 0);
    }
    return r;
}

void max_heapify_push(struct heap_entity *ptr, int v) {
    ptr->ptr[ptr->n++] = v;
    max_heapify_percolate_up(ptr, ptr->n-1);
}

void max_heapify_increase_key(struct heap_entity *ptr, int i) {
    max_heapify_percolate_up(ptr, i);
}

void max_heapify_decrease_key(struct heap_entity *ptr, int i) {
    max_heapify(ptr, i);
}
