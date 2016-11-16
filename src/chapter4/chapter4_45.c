#include "chapter4/chapter4_45.h"

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "util.h"

struct tree_node45 {
    struct tree_node45 *l, *r;
    int data;
};

static struct tree_node45 *alloc_tree_node45(int data) {
    struct tree_node45 *node = (struct tree_node45*) malloc(sizeof(struct tree_node45));
    node->l = node->r = NULL;
    node->data = data;
    return node;
}

/**
 * prev是node节点及其子树的所有节点的前向节点
 * next是node节点及其子树的所有节点的后行节点
 * 如果node->l等于prev,那么node->l认为是空的, 如果node->r等于next,那么认为node->r是空的
 *
 */
static struct tree_node45 *insert_tree_node45_recursive(struct tree_node45 *node, int data,
        struct tree_node45 *prev, struct tree_node45 *next) {
    struct tree_node45 *temp;
    if (!node) {
        node = alloc_tree_node45(data);
        node->l = prev;
        node->r = next;
    } else if (data < node->data) {
        if (node->l == prev) {
            temp = alloc_tree_node45(data);
            node->l = temp;
            temp->l = prev;
            temp->r = node;
        } else {
            node->l = insert_tree_node45_recursive(node->l, data, prev, node);
        }
    } else if (data > node->data) {
        if (node->r == next) {
            temp = alloc_tree_node45(data);
            node->r = temp;
            temp->l = node;
            temp->r = next;
        } else {
            node->r = insert_tree_node45_recursive(node->r, data, node, next);
        }
    }
    return node;
}

static struct tree_node45 *insert_tree_node45(struct tree_node45 *root, int data) {
    return insert_tree_node45_recursive(root, data, NULL, NULL);
}

static void release_trees45_recursive(struct tree_node45 *node, struct tree_node45 *prev, 
        struct tree_node45 *next) {
    if (node) {
        if (node->l != prev) {
            release_trees45_recursive(node->l, prev, node);
        }
        if (node->r != next) {
            release_trees45_recursive(node->r, node, next);
        }
        free(node);
    }
}

static void release_trees45(struct tree_node45 *node) {
    release_trees45_recursive(node, NULL, NULL);
}

static struct tree_node45 *find_max45(struct tree_node45 *node, struct tree_node45 *prev,
        struct tree_node45 *next) {
    if (node->r != next) {
        return find_max45(node->r, node, next);
    }
    return node;
}

static struct tree_node45 *delete_tree_node45_recursive(struct tree_node45 *node, int data,
        struct tree_node45 *prev, struct tree_node45 *next) {
    struct tree_node45 *temp;
    if (node) {
        if (data < node->data) {
            node->l = delete_tree_node45_recursive(node->l, data, prev, node);
            if (node->l == node) {
                node->l = prev;
            }
        } else if (data > node->data) {
            node->r = delete_tree_node45_recursive(node->r, data, node, next);
            if (node->r == node) {
                node->r = next;
            }
        } else if (node->l != prev && node->r != next) {
            temp = find_max45(node->r, node, next);
            node->data = temp->data;
            node->r = delete_tree_node45_recursive(node->r, temp->data, node, next);
            if (node->r == node) {
                node->r = next;
            }
        } else {
            temp = node;
            if (node->l != prev) {
                node = node->l;
                node->r = next;
            } else if (node->r != next) {
                node = node->r;
                node->l = prev;
            } else {
                node = node->l;
            }
        }
    }
    return node;
}

static struct tree_node45 *delete_tree_node45(struct tree_node45 *node, int data) {
    return delete_tree_node45_recursive(node, data, NULL, NULL);
}

void chapter4_45_problem_b() {
    int data_buf[] = {7, 4, 2, 1, 3, 6, 5, 13, 11, 9, 8, 12, 10, 15, 14, 16};
    struct tree_node45 *root;

    root = NULL;
    for (int i = 0; i < ARRAY_SIZE(data_buf); ++i) {
        root = insert_tree_node45(root, data_buf[i]);
    }
    for (int i = ARRAY_SIZE(data_buf) - 1; i >= 0; --i) {
        root = delete_tree_node45(root, data_buf[i]);
    }
    release_trees45(root);
    root = NULL;
}