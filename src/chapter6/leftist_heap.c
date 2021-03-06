#include "chapter6/leftist_heap.h"

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "macros.h"
#include "util.h"

struct leftist_heap_node {
    int key;
    int npl;
    struct leftist_heap_node *left;
    struct leftist_heap_node *right;
};

#define NPL(node) ((node) ? (node)->npl : -1)

static inline int npl(struct leftist_heap_node *node) {
    return node == NULL ? -1 : MIN(npl(node->left), npl(node->right)) + 1;
}

static struct leftist_heap_node *alloc_leftist_heap_node(int key) {
    struct leftist_heap_node *node;

    node = (struct leftist_heap_node*) malloc(sizeof(struct leftist_heap_node));
    if (!node) {
        fatal_error("alloc_leftist_heap_node fail");
    }
    node->key = key;
    node->left = node->right = NULL;
    node->npl = npl(node);
    return node;
}

static void swap(struct leftist_heap_node *node) {
    struct leftist_heap_node *temp;
    temp = node->left;
    node->left = node->right;
    node->right= temp;
}

/**
 * 两个左倾堆的合并
 */
static struct leftist_heap_node *merge(struct leftist_heap_node *k1, 
        struct leftist_heap_node *k2) {
    if (k1 == NULL) {
        return k2;
    } else if (k2 == NULL) {
        return k1;
    } else if (k1->key < k2->key) {
        k1->right = merge(k1->right, k2);
        if (NPL(k1->left) < NPL(k1->right)) {
            swap(k1);
        }
        k1->npl = npl(k1);
        return k1;
    } else {
        k2->right = merge(k2->right, k1);
        if (NPL(k2->left) < NPL(k2->right)) {
            swap(k2);
        }
        k2->npl = npl(k2);
        return k2;
    }
}

static struct leftist_heap_node *leftist_heap_enqueue(struct leftist_heap_node *heap,
    int key) {
    return  merge(heap, alloc_leftist_heap_node(key));
}

static struct leftist_heap_node *leftist_heap_dequeue(struct leftist_heap_node *heap) {
    struct leftist_heap_node *node;

    DCHECK(heap);
    node = heap;
    heap = merge(node->left, node->right);
    free(node);
    return heap;
}

static void dump_leftist_heap_inorder(struct leftist_heap_node *heap) {
    while (heap != NULL) {
        printf("%d ", heap->key);
        heap = leftist_heap_dequeue(heap);
    }
    printf("\n");
}

static void free_leftist_heap_node(struct leftist_heap_node *node) {
    if (node != NULL) {
        free_leftist_heap_node(node->left);
        free_leftist_heap_node(node->right);
        free(node);
    }
}

void chapter6_6_tutorial() {
    struct leftist_heap_node *heap;
    int data_buf[] = { 10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2 };

    heap = NULL;
    for (int i = 0; i < NARRAY(data_buf); ++i) {
        heap = leftist_heap_enqueue(heap, data_buf[i]);
    }
    dump_leftist_heap_inorder(heap);
    heap = NULL;
}

void chapter6_16_problem() {
    int k1_data[] = {11, 12, 17, 18, 2, 5, 8, 15};
    int k2_data[] = {31, 18, 9, 10, 4, 6, 11, 21};
    struct leftist_heap_node *k1, *k2;

    k1 = NULL;
    k2 = NULL;
    for (int i = 0; i < NARRAY(k1_data); ++i) {
        k1 = leftist_heap_enqueue(k1, k1_data[i]);
    }
    for (int i = 0; i < NARRAY(k2_data); ++i) {
        k2 = leftist_heap_enqueue(k2, k2_data[i]);   
    }
    k1 = merge(k1, k2);
    dump_leftist_heap_inorder(k1);
    k1 = NULL;
    k2 = NULL;
    free_leftist_heap_node(k1);
    free_leftist_heap_node(k2);
}

void chapter6_17_problem() {
    struct leftist_heap_node *node;
    node = NULL;
    for (int i = 1; i <= 15; ++i) {
        node = leftist_heap_enqueue(node, i);
    }
    dump_leftist_heap_inorder(node);
    node = NULL;
}