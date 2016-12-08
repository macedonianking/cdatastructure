#include "chapter6/binomial_queue.h"

#include "list.h"
#include "util.h"
#include "macros.h"


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
struct bin_queue_node_t *alloc_bin_queue_node(int key) {
    struct bin_queue_node_t *ptr = (struct bin_queue_node_t*) malloc(sizeof(*ptr));
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
void init_binomial_queue(struct binomial_queue *queue, int capacity) {
    if (capacity <= 0) {
        capacity = 10;
    }
    queue->queue = (struct bin_queue_node_t**) malloc(sizeof(struct bin_queue_node_t*));
    DCHECK(queue->queue);
    memset(queue->queue, 0, sizeof(struct bin_queue_node_t*) * capacity);
    queue->capacity = capacity;
}

/**
 * Free the entire binomial_queue elements.
 */
void free_binomial_queue(struct binomial_queue *queue) {
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

void chapter6_8_tutorial() {

}