#include "chapter6/algorithm6_5.h"

#include "macros.h"
#include "util.h"

struct heap_t {
    int *queue;
    int size;
    int capapcity;
};

#define HEAP_LEFT(x)    ((x)*2 + 1)
#define HEAP_RIGHT(x)   ((x)*2 + 2)
#define HEAP_PARENT(x)  (((x)-1)/2)

/**
 * 最大堆数据下降
 */
static void max_heapify(struct heap_t *heap, int i) {
    int value;
    int temp;
    int curr_v, curr_i;
    
    DCHECK(i < heap->size);
    value = heap->queue[i];
    while (i < heap->size) {
        curr_i = i;
        curr_v = value;
        temp = HEAP_LEFT(i);
        if (temp < heap->size && heap->queue[temp] > curr_v) {
            curr_i = temp;
            curr_v = heap->queue[temp];
        }
        temp = HEAP_RIGHT(i);
        if (temp < heap->size && heap->queue[temp] > curr_v) {
            curr_i = temp;
            curr_v = heap->queue[temp];
        }
        if (i == curr_i) {
            break;
        }
        heap->queue[i] = curr_v;
        i = curr_i;
    }
    heap->queue[i] = value;
}

/**
 * 最小堆数据下降
 */
static void min_heapify(struct heap_t *heap, int i) {
    int value;
    int curr_i, curr_v;
    int index;

    DCHECK(i < heap->size);
    value = heap->queue[i];
    while (i < heap->size) {
        curr_i = i;
        curr_v = value;
        index = HEAP_LEFT(i);
        if (index < heap->size && heap->queue[index] < curr_v) {
            curr_i = index;
            curr_v = heap->queue[index];
        }
        index = HEAP_RIGHT(i);
        if (index < heap->size && heap->queue[index] < curr_v) {
            curr_i = index;
            curr_v = heap->queue[index];
        }
        if (i == curr_i) {
            break;
        }
        heap->queue[i] = curr_v;
        i = curr_i;
    }
    heap->queue[i] = value;
}

/**
 * 构建最大堆
 */
static void build_max_heap(struct heap_t *heap) {
    for(int i = HEAP_PARENT(heap->size-1); i >= 0; --i) {
        max_heapify(heap, i);
    }
}

/**
 * 构建最小堆
 */
static void build_min_heap(struct heap_t *heap) {
    for (int i = HEAP_PARENT(heap->size-1); i>=0; --i) {
        min_heapify(heap, i);
    }
}

#if 0

/**
 * 最大堆数据上升
 */
static void max_heap_percolate_up(struct heap_t *heap, int i) {
    int v;

    DCHECK(i < heap->size);
    v = heap->queue[i];
    for (int p = HEAP_PARENT(i); p >= 0 && v > heap->queue[p]; i = p) {
        heap->queue[i] = heap->queue[p];
    }
    heap->queue[i] = v;
}

/**
 * 最小堆数据上升
 */
static void min_heap_percolate_up(struct heap_t *heap, int i) {
    int v;

    DCHECK(i < heap->size);
    v = heap->queue[i];
    for (int p = HEAP_PARENT(i); p >= 0 && v < heap->queue[p]; i = p) {
        heap->queue[i] = heap->queue[p];
    }
    heap->queue[i] = v;
}

#endif 

/**
 * 最大堆获取最大值
 */
static int max_heap_dequeue(struct heap_t *heap) {
    int n;

    DCHECK(heap->size > 0);
    n = heap->queue[0];
    if (--heap->size > 0) {
        heap->queue[0] = heap->queue[heap->size];
        max_heapify(heap, 0);
    }
    return n;
}

/**
 * 最小堆获取最小值
 */
static int min_heap_dequeue(struct heap_t *heap) {
    int n;

    DCHECK(heap->size > 0);
    n = heap->queue[0];
    if (--heap->size > 0) {
        heap->queue[0] = heap->queue[heap->size];
        min_heapify(heap, 0);
    }
    return n;
}

static void algorithm6_5_1_problem_a() {
    int buf[] = {15, 13, 9, 5, 12, 8, 7, 4, 0, 6, 2, 1};
    struct heap_t heap;
    
    heap.size = NARRAY(buf);
    heap.queue = buf;
    build_max_heap(&heap);

    while (heap.size > 0) {
        fprintf(stdout, "%d ", max_heap_dequeue(&heap));
    }
    fputc('\n', stdout);
}

static void algorithm6_5_1_problem_b() {
    int buf[] = {15, 13, 9, 5, 12, 8, 7, 4, 0, 6, 2, 1};
    struct heap_t heap;

    heap.size = NARRAY(buf);
    heap.queue = buf;
    build_min_heap(&heap);

    while (heap.size > 0) {
        fprintf(stdout, "%d ", min_heap_dequeue(&heap));
    }
    fputc('\n', stdout);
}

void algorithm6_5_1_problem() {
    algorithm6_5_1_problem_a();
    algorithm6_5_1_problem_b();
}

