#include "chapter6/binomial_queue.h"

#include "list.h"
#include "util.h"
#include "macros.h"
#include "log.h"

struct bin_queue_node_t {
    struct list_head node;
    struct list_head list;
    int              key;
};

struct binomial_queue {
    struct bin_queue_node_t **queue;
    int                     capacity;
};

/**
 * Allocate memory for a bin_queue_node_t object.
 */
static struct bin_queue_node_t *alloc_bin_queue_node(int key) {
    struct bin_queue_node_t *ptr = (struct bin_queue_node_t*) malloc(sizeof(struct bin_queue_node_t));
    DCHECK(ptr);
    INIT_LIST_HEAD(&ptr->node);
    INIT_LIST_HEAD(&ptr->list);
    ptr->key = key;
    return ptr;
}

/**
 * Free memory for bin_queue_node_t object
 */
static void free_bin_queue_node(struct bin_queue_node_t *ptr) {
    DCHECK(list_empty(&ptr->node));
    DCHECK(list_empty(&ptr->list));
    free(ptr);
}

static void free_bin_queue_node_trees(struct bin_queue_node_t *node) {
    struct bin_queue_node_t *ptr;
    if (!node) {
        return;
    }
    LIST_FOR_EACH_ENTRY_SAFE(ptr, &node->list, node) {
        list_del(&ptr->node);
        free_bin_queue_node_trees(ptr);
    }
    free_bin_queue_node(node);
}

/**
 * Merge two bin_queue_node_t object.
 */
struct bin_queue_node_t *merge(struct bin_queue_node_t *k1, struct bin_queue_node_t *k2) {
    struct bin_queue_node_t *temp;
    if (k2->key < k1->key) {
        temp = k2;
        k2 = k1;
        k1 = temp;
    }
    list_add_tail(&k2->node, &k1->list);
    return k1;
}

/**
 * Initialize the binomial_queue data structure with capacity value.
 */
static void init_binomial_queue(struct binomial_queue *queue, int capacity) {
    if (capacity <= 0) {
        capacity = 10;
    }
    queue->queue = (struct bin_queue_node_t**) malloc(sizeof(struct bin_queue_node_t*) * capacity);
    DCHECK(queue->queue);
    memset(queue->queue, 0, sizeof(struct bin_queue_node_t*) * capacity);
    queue->capacity = capacity;
}

/**
 * Free the entire binomial_queue elements.
 */
static void free_binomial_queue(struct binomial_queue *queue) {
    struct bin_queue_node_t *ptr;
    for (int i = 0; i < queue->capacity; ++i) {
        if (queue->queue[i] != NULL) {
            ptr = queue->queue[i];
            queue->queue[i] = NULL;
            free_bin_queue_node_trees(ptr);
        }
    }
    free(queue->queue);
    queue->queue = NULL;
    queue->capacity = 0;
}

/**
 * 检查当前的capacity的值是否合适
 */
static void check_binomial_queue_capacity(struct binomial_queue *queue,
                                          int capacity) {
    int next;
    struct bin_queue_node_t **ptr;
    if (capacity <= queue->capacity) {
        return;
    }
    next = next_capacity(queue->capacity);
    if (next < capacity) {
        next = capacity;
    }
    ptr = (struct bin_queue_node_t**) realloc(queue->queue, next * sizeof(struct bin_queue_node_t*));
    if (!ptr) {
        fatal_error("check_binomial_queue_capacity failed");
    }
    for (int i = queue->capacity; i < next; ++i) {
        ptr[i] = NULL;
    }
    queue->queue = ptr;
    queue->capacity = next;
}

static void add_binomial_queue(struct binomial_queue *queue, 
                               struct bin_queue_node_t *node, 
                               int order) {
    DCHECK(node && order >= 0);
    struct bin_queue_node_t *ptr;
    check_binomial_queue_capacity(queue, order + 1);
    if (queue->queue[order] == NULL) {
        queue->queue[order] = node;
    } else{
        ptr = queue->queue[order];
        queue->queue[order] = NULL;
        node = merge(node, ptr);
        add_binomial_queue(queue, node, order + 1);
    }
}

static int binomial_queue_find_min(struct binomial_queue *queue) {
    struct bin_queue_node_t *ptr, **min;
    min = NULL;
    for (int i = 0; i < queue->capacity; ++i) {
        ptr = queue->queue[i];
        if (ptr != NULL && (min == NULL || (ptr->key) < (*min)->key)) {
            min = queue->queue + i;
        }
    }
    return min == NULL ? -1 : min - queue->queue;
}

static int binomial_queue_is_empty(struct binomial_queue *queue) {
    int is_empty = 1;
    for (int i = 0; i < queue->capacity && is_empty; ++i) {
        is_empty = queue->queue[i] == NULL;
    }
    return is_empty;
}

static void binomial_queue_enqueue(struct binomial_queue *queue, int key) {
    struct bin_queue_node_t *node = alloc_bin_queue_node(key);
    add_binomial_queue(queue, node, 0);
}

// 取出最小值，将他的子成员放到内部
static int binomial_queue_dequeue(struct binomial_queue *queue) {
    int min_index, order;
    int min;
    struct bin_queue_node_t *node, *ptr;
    DCHECK(!binomial_queue_is_empty(queue));
    min_index = binomial_queue_find_min(queue);
    DCHECK(min_index != -1);
    
    node = queue->queue[min_index];
    queue->queue[min_index] = NULL;
    min = node->key;
    order = 0;
    LIST_FOR_EACH_ENTRY_SAFE(ptr, &node->list, node) {
        list_del(&ptr->node);
        add_binomial_queue(queue, ptr, order);
        ++order;
    }
    free(node);

    return min;   
}

static int binomial_queue_node_percolate_up(struct bin_queue_node_t *node, int key) {
    if (node == NULL || node->key > key) {
        return 0;
    } else if (node->key == key) {
        return 1;
    } else {
        struct bin_queue_node_t *ptr, *dst;
        int temp;
        dst = NULL;
        LIST_FOR_EACH_ENTRY(ptr, &node->list, node) {
            if (binomial_queue_node_percolate_up(ptr, key)) {
                dst = ptr;
                break;
            }
        }
        if (dst != NULL) {
            temp = node->key;
            node->key = dst->key;
            dst->key = temp;
            return 1;
        }
        return 0;
    }
}

int binomial_queue_remove(struct binomial_queue *queue, int key) {
    struct bin_queue_node_t *dst, *ptr;
    int order, result;
    dst = NULL;
    for (int i = 0; i < queue->capacity; ++i) {
        ptr = queue->queue[i];
        if (ptr != NULL && binomial_queue_node_percolate_up(ptr, key)) {
            DCHECK(ptr->key == key);
            dst = ptr;
            queue->queue[i] = NULL;
            break;
        }
    }
    result = 0;
    if (dst == NULL) {
        goto out;
    }
    result = 1;
    order = 0;
    LIST_FOR_EACH_ENTRY_SAFE(ptr, &dst->list, node) {
        list_del(&ptr->node);
        add_binomial_queue(queue, ptr, order);
        ++order;
    }
    free(dst);

out:
    return result;
}

/*
* Merge two binomial queue
*/
void binomial_queue_merge(struct binomial_queue *dst, struct binomial_queue *src) {
    struct bin_queue_node_t *ptr;
    for (int i = 0; i < src->capacity; ++i) {
        ptr = src->queue[i];
        src->queue[i] = NULL;
        if (ptr != NULL) {
            add_binomial_queue(dst, ptr, i);
        }
    }
}

static void dump_binomial_queue_inorder(struct binomial_queue *queue) {
    while (!binomial_queue_is_empty(queue)) {
        fprintf(stdout, "%d ", binomial_queue_dequeue(queue));
    }
    fputc('\n', stdout);
}

void chapter6_8_tutorial() {
    struct binomial_queue queue;
    
    init_binomial_queue(&queue, 10);
    for (int i = 0; i < 8; ++i) {
        binomial_queue_enqueue(&queue, i);
    }
    dump_binomial_queue_inorder(&queue);
    free_binomial_queue(&queue);
}