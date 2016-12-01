#include "chapter6/binary_heap.h"

#include <stdlib.h>

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

void init_binary_head(struct binary_heap *heap, int c) {
    DCHECK(c > 0);
    heap->queue = (int*) malloc(sizeof(int) * c);
    DCHECK(heap->queue);
    heap->size = 0;
    heap->capacity = c;
}

void extend_binary_heap(struct binary_heap *heap, int c) {
    int *queue;

    queue = (int*) realloc(heap->queue, sizeof(int) * c);
    if (!queue) {
        fatal_error("extend_binary_heap failure");
    }
    heap->queue = queue;
    heap->capacity = c;
}

void binary_heap_enqueue(struct binary_heap *heap, int value) {
    int index;
    
    if (heap->capacity == heap->size) {
        extend_binary_heap(heap, next_capacity(heap->capacity));
    }

    index = heap->size;
    while (index != 0 && heap->queue[heap_parent(index)] > value) {
        heap->queue[index] = heap->queue[heap_parent(index)];
        index = heap_parent(index);
    }
    heap->queue[index] = value;
    ++heap->size;
}

/*
入队的操作
*/
int binary_heap_dequeue(struct binary_heap *heap) {
    int r;
    int index;
    int temp_index, next_index;
    int value, next_value;

    DCHECK(heap->size);
    r = heap->queue[0];
    --heap->size;
    if (heap->size > 0) {
        index = 0;
        value = heap->queue[heap->size];
        while (index < heap->size) {
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

    return r;   
}

void chapter6_3_tutorial() {

}