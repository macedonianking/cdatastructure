#include "chapter6/dheap.h"

#include <stdio.h>
#include <stdlib.h>

struct dheap_t {
    int *queue;
    int capacity;
    int size;
    int d;
};

#define dheap_child_start(x, d) ((x) * (d) + 1)
#define dheap_parent(x, d)      (((x)-1) / (d))

void dump_dheap_parent_and_children(struct dheap_t *heap, int i) {
    int start;
    printf("dheap %d parent:", i);
    if (i != 0) {
        printf(" %d\n", dheap_parent(i, heap->d));
    }
    printf("dheap %d children:", i);
    start = dheap_child_start(i, heap->d);
    for (int i = 0; i < heap->d && start < heap->size; ++i, ++start) {
        printf(" %d", start);
    }
}

void chapter6_13_problem() {

}