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
    int next_index, next_value;
    int temp_index;
    int value;

    value = heap->queue[i];
    while (i < heap->size) {
        next_index = i;
        next_value = value;
        // 查找最大值
        temp_index = HEAP_LEFT(i);
        if (temp_index < heap->size && heap->queue[temp_index] > next_value) {
            next_index = temp_index;
            next_value = heap->queue[temp_index];
        }
        temp_index = HEAP_RIGHT(i);
        if (temp_index < heap->size && heap->queue[temp_index] > next_value) {
            next_index = temp_index;
            next_value = heap->queue[temp_index];
        }
        if (next_index == i) {
            break;
        }
        heap->queue[i] = next_value;
        i = next_index;
    }
    heap->queue[i] = value;
}

/**
 * 最小堆数据下将
 * 如果数据大，那么需要下降
 */
static void min_heapify(struct heap_t *heap, int i) {
    int next_index, next_value;
    int temp_index;
    int value;

    value = heap->queue[i];
    while (i < heap->size) {
        next_index = i;
        next_value = value;
        // 查找最小值
        temp_index = HEAP_LEFT(i);
        if (temp_index < heap->size && heap->queue[temp_index] < next_value) {
            next_index = temp_index;
            next_value = heap->queue[temp_index];
        }
        temp_index = HEAP_RIGHT(i);
        if (temp_index < heap->size && heap->queue[temp_index] < next_value) {
            next_index = temp_index;
            next_value = heap->queue[temp_index];
        }
        if (i == next_index) {
            break;
        }
        heap->queue[i] = next_value;
        i = next_index;
    }
    heap->queue[i] = value;
}

static void build_max_heap(struct heap_t *heap) {
    for (int i = HEAP_PARENT(heap->size-1); i >= 0; --i) {
        max_heapify(heap, i);
    }
}

static void build_min_heap(struct heap_t *heap) {
    for (int i = HEAP_PARENT(heap->size-1); i >= 0; --i) {
        min_heapify(heap, i);
    }
}

static void heap_sort_asc(int *buf, int n) {
    struct heap_t heap;
    int temp;

    heap.queue = buf;
    heap.size = n;
    heap.capacity = n;
    build_max_heap(&heap);

    while (heap.size > 0) {
        temp = heap.queue[0];
        heap.queue[0] = heap.queue[heap.size-1];
        heap.queue[heap.size-1] = temp;
        if (--heap.size > 0) {
            max_heapify(&heap, 0);
        }
    }
}

static void heap_sort_dsc(int *buf, int n) {
    struct heap_t heap;
    int temp;

    heap.queue = buf;
    heap.size = n;
    heap.capacity = n;
    build_min_heap(&heap);

    while (heap.size > 0) {
        temp = heap.queue[0];
        heap.queue[0] = heap.queue[heap.size-1];
        heap.queue[heap.size-1] = temp;
        if (--heap.size > 0) {
            min_heapify(&heap, 0);
        }
    }
}

void algorithm6_2_tutorial() {
    struct heap_t heap;
    int buf[] = {16, 4, 10, 14, 7, 9, 3, 2, 8, 1};

    init_heap(&heap, NARRAY(buf));
    memcpy(heap.queue, buf, NARRAY(buf) * sizeof(int));
    heap.size = NARRAY(buf);
    max_heapify(&heap, 1);
    dump_narray(heap.queue, heap.size);
    free_heap(&heap);
}

void algorithm6_2_1_problem() {
    int buf[] = {27, 17, 3, 16, 13, 10, 1, 5, 7, 12, 4, 8, 9, 0};
    struct heap_t heap;

    init_heap(&heap, NARRAY(buf));
    memcpy(heap.queue, buf, NARRAY(buf) * sizeof(int));
    heap.size = NARRAY(buf);
    max_heapify(&heap, 2);
    dump_narray(heap.queue, heap.size);
    free_heap(&heap);
}

void algorithm6_2_2_problem() {
    int buf[] = {27, 17, 3, 16, 13, 10, 1, 5, 7, 12, 4, 8, 9, 0};
    struct heap_t heap;

    heap.queue = buf;
    heap.size = NARRAY(buf);
    min_heapify(&heap, 0);
    dump_narray(heap.queue, heap.size);
}

// 构建最大堆
void algorithm6_3_1_problem() {
    struct heap_t heap;
    int buf[] = {5, 3, 17, 10, 84, 19, 6, 22, 9};
    
    heap.queue = buf;
    heap.size = NARRAY(buf);
    build_max_heap(&heap);
    dump_narray(heap.queue, heap.size);
}

void algorithm6_4_tutorial() {
    int buf[] = {5, 3, 17, 10, 84, 19, 6, 22, 9};

    heap_sort_asc(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
    heap_sort_dsc(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}