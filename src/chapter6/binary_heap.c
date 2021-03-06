#include "chapter6/binary_heap.h"

#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include "macros.h"
#include "log.h"
#include "util.h"

#define heap_parent(x)  (((x) - 1) >> 1)
#define heap_left(x)    (((x)<<1)+1)
#define heap_right(x)   (((x)<<1)+2)

struct binary_heap {
    int *queue;
    int size;
    int capacity;
};

static void percolate_down(struct binary_heap *heap, int index);
static void percolate_up(struct binary_heap *heap, int index);
static void build_heap(struct binary_heap *heap);

static void init_binary_head(struct binary_heap *heap, int c) {
    DCHECK(c > 0);
    heap->queue = (int*) malloc(sizeof(int) * c);
    DCHECK(heap->queue);
    heap->size = 0;
    heap->capacity = c;
}

static void free_binary_heap(struct binary_heap *heap) {
    free(heap->queue);
    heap->queue = NULL;
    heap->size = heap->capacity = 0;
}

static void extend_binary_heap(struct binary_heap *heap, int c) {
    int *queue;

    queue = (int*) realloc(heap->queue, sizeof(int) * c);
    if (!queue) {
        fatal_error("extend_binary_heap failure");
    }
    heap->queue = queue;
    heap->capacity = c;
}

static void binary_heap_enqueue(struct binary_heap *heap, int value) {
    if (heap->capacity == heap->size) {
        extend_binary_heap(heap, next_capacity(heap->capacity));
    }

    heap->queue[heap->size++] = value;
    percolate_up(heap, heap->size-1);
}

/*
入队的操作
*/
int binary_heap_dequeue(struct binary_heap *heap) {
    int r;
    DCHECK(heap->size);
    r = heap->queue[0];
    --heap->size;
    if (heap->size > 0) {
        heap->queue[0] = heap->queue[heap->size];
        percolate_down(heap, 0);
    }
    return r;   
}

/*
* heap成员升级
*/
static void percolate_up(struct binary_heap *heap, int index) {
    DCHECK(index >= 0 && index < heap->size);
    int value;

    value = heap->queue[index];
    while (index != 0 && value < heap->queue[heap_parent(index)]) {
        heap->queue[index] = heap->queue[heap_parent(index)];
        index = heap_parent(index);
    }
    heap->queue[index] = value;
}

/*
* heap成员降级
*/
static void percolate_down(struct binary_heap *heap, int index) {
    int value;
    int next_index, next_value;
    int temp_index;

    DCHECK(index >= 0 && index < heap->size);
    value = heap->queue[index];
    for (;;) {
        next_index = index;
        next_value = value;

        temp_index = heap_left(index);
        if (temp_index < heap->size && heap->queue[temp_index] < next_value) {
            next_index = temp_index;
            next_value = heap->queue[temp_index];
        }
        temp_index = heap_right(index);
        if (temp_index < heap->size && heap->queue[temp_index] < next_value) {
            next_index = temp_index;
            next_value = heap->queue[temp_index];
        }
        if (index == next_index) {
            break;
        }
        heap->queue[index] = heap->queue[next_index];
        index = next_index;
    }
    heap->queue[index] = value;
}

void build_heap(struct binary_heap *heap) {
    DCHECK(heap->size > 0);
    for (int index = heap_parent(heap->size-1); index >= 0; --index) {
        percolate_down(heap, index);
    }
}

static void binary_heap_increase_key(struct binary_heap *heap, int index, int delta) {
    DCHECK(index >= 0 && index < heap->size);
    DCHECK(delta >= 0);
    heap->queue[index] += delta;
    percolate_down(heap, index);
}

static void binary_heap_decrease_key(struct binary_heap *heap, int index, int delta) {
    DCHECK(index >= 0 && index < heap->size);
    DCHECK(delta >= 0);
    heap->queue[index] -= delta;
    percolate_up(heap, index);
}

static void dump_binary_heap_inorder(struct binary_heap *heap) {
    while (heap->size) {
        printf("%d ", binary_heap_dequeue(heap));
    }
}

static int binary_heap_delete(struct binary_heap *heap, int index) {
    int r;
    DCHECK(index >= 0 && index < heap->size);
    r = heap->queue[index];
    if (--heap->size && index < heap->size) {
        heap->queue[index] = heap->queue[heap->size];
        percolate_down(heap, index);
    }
    return r;
}

void chapter6_3_tutorial() {
}

/**
 * Build binary heap from insert method.
 */
void chapter6_2_a_problem() {
    int data_buf[] = { 10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2 };
    struct binary_heap heap;

    init_binary_head(&heap, 128);
    for (int i = 0; i < NARRAY(data_buf); ++i) {
        binary_heap_enqueue(&heap, data_buf[i]);
    }
    free_binary_heap(&heap);
}

/**
 * Build binary heap from linear-timed algorithm.
 */
void chapter6_2_b_problem() {
    int data_buf[] = { 10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2 };
    struct binary_heap heap;

    init_binary_head(&heap, 128);
    memcpy(heap.queue, data_buf, sizeof(data_buf));
    heap.size = NARRAY(data_buf);
    build_heap(&heap);
    dump_binary_heap_inorder(&heap);
    free_binary_heap(&heap);
}

/**
 * Build binary heap and delete 3 elements.
 */
void chapter6_3_problem() {
    int data_buf[] = { 10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2 };
    struct binary_heap heap;

    init_binary_head(&heap, 128);
    heap.size = NARRAY(data_buf);
    memcpy(heap.queue, data_buf, sizeof(data_buf));
    for (int i = 0; i < 3; ++i) {
        if (heap.size > 0) {
            printf("heap min:%d\n", binary_heap_dequeue(&heap));
        }
    }
    dump_binary_heap_inorder(&heap);
    free_binary_heap(&heap);
}

void chapter6_5_problem() {
    int data_buf[] = { 10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2 };
    struct binary_heap heap;

    init_binary_head(&heap, 128);
    for (int i = 0; i < NARRAY(data_buf); ++i) {
        heap.queue[i] = data_buf[i];
    }
    heap.size = NARRAY(data_buf);
    build_heap(&heap);
    binary_heap_increase_key(&heap, 3, 10);
    binary_heap_decrease_key(&heap, 5, 2);
    binary_heap_delete(&heap, 3);
    dump_binary_heap_inorder(&heap);
    printf("\n");

    free_binary_heap(&heap);
}

void chapter6_6_problem() {
    int sum = 0;
    int value = 1;
    int depth = 0;
    do {
        sum += value;
        value <<= 1;
    } while(++depth < 7);
    sum -= 2 * 15;
    printf("node count: %d", sum);
}

static void chapter6_8_a_problem_internal(struct binary_heap *heap, int index, int key,
    void (*func)(int value)) {
    if (index < 0 && index >= heap->size) {
        return;
    }
    if (heap->queue[index] >= key) {
        return;
    }
    func(heap->queue[index]);
    chapter6_8_a_problem_internal(heap, heap_left(index), key, func);
    chapter6_8_a_problem_internal(heap, heap_right(index), key, func);
}

void chapter6_8_a_problem(struct binary_heap *heap, int key, void (*func)(int value)) {
    DCHECK(func);
    chapter6_8_a_problem_internal(heap, 0, key, func);
}