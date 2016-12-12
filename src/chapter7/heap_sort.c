#include "chapter7/heap_sort.h"

#include "macros.h"
#include "util.h"

#define heap_left(x)    (((x)*2) + 1)
#define heap_right(x)   ((x) *2 + 2)
#define heap_parent(x)  (((x)-1)/2)

// 最大堆percolate down.
static void percolate_down(int *buf, int n, int i) {
    int next_index;
    int next_value, temp;
    int temp_index;
    DCHECK(i < n);
    
    temp = buf[i];
    while (1) {
        next_index = i;
        next_value = temp;
        temp_index = heap_left(i);
        if (temp_index < n && buf[temp_index] > next_value) {
            next_index = temp_index;
            next_value = buf[next_index];
        }
        temp_index = heap_right(i);
        if (temp_index < n && buf[temp_index] > next_value) {
            next_index = temp_index;
            next_value = buf[next_index];
        }
        if (i == next_index) {
            break;
        }
        buf[i] = buf[next_index];
        i = next_index;
    }
    buf[i] = temp;
}

// 构建一个最大堆
static void build_heap(int *buf, int n) {
    for (int i = heap_parent(n-1); i>= 0; --i) {
        percolate_down(buf, n, i);
    }
}

void heap_sort(int *buf, int n) {
    int j;
    int temp;
    build_heap(buf, n);
    for (j = n - 1; j > 0; j--) {
        temp = buf[j];
        buf[j] = buf[0];
        buf[0] = temp;
        --n;
        percolate_down(buf, n, 0);
    }
}

void chapter7_11_problem() {
    int buf[] = {142, 543, 123, 65, 453, 879, 572, 434, 111, 242, 811, 102};
    heap_sort(buf, NARRAY(buf));
    dump_narray(buf, NARRAY(buf));
}