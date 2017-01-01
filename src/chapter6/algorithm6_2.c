#include "chapter6/algorithm6_2.h"

#include <stdlib.h>

#include "macros.h"
#include "util.h"

struct heap_t {
    int *queue;
    int size;
    int capacity;
};

#define DEFAULT_HEAP_CAPACITY   54
#define HEAP_PARENT(x)          (((x)-1)>>1)
#define HEAP_LEFT(x)            ((x)*2 + 1)
#define HEAP_RIGHT(x)           ((x)*2 + 2)

static void init_heap(struct heap_t *heap, int c) {
    if (c <= 0) {
        c = DEFAULT_HEAP_CAPACITY;
    }
    heap->queue = (int*) malloc(c * sizeof(int));
    DCHECK(heap->queue);
    heap->size = 0;
    heap->capacity = c;
}

static void free_heap(struct heap_t *heap) {
    free(heap->queue);
    heap->queue = NULL;
    heap->size = 0;
    heap->capacity = 0;
}

/**
 * 最大堆数据下降
 */
static void max_heapify(struct heap_t *heap, int i) {
    int next_index;
    int temp_index;
    int value;

    value = heap->queue[i];
    while (i < heap->size) {
        next_index = i;
        temp_index = HEAP_LEFT(i);
        if (temp_index < heap->size 
            && heap->queue[temp_index] > heap->queue[next_index]) {
            next_index = temp_index;
        }
        temp_index = HEAP_RIGHT(i);
        if (temp_index < heap->size 
            && heap->queue[temp_index] > heap->queue[next_index]) {
            next_index = temp_index;
        }
        if (next_index == i) {
            break;
        }
        heap->queue[i] = heap->queue[next_index];
        i = next_index;
    }
    heap->queue[i] = value;
}

void algorithm6_2_tutorial() {
    struct heap_t heap;
    int buf[] = {16, 4, 10, 14, 7, 9, 3, 2, 8, 1};

    init_heap(&heap, NARRAY(buf));
    memcpy(heap.queue, buf, NARRAY(buf) * sizeof(int));
    heap.size = NARRAY(buf);
    max_heapify(&heap, 1);
    free_heap(&heap);
}


