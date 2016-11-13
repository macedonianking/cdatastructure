#include "chapter4/chapter4_44.h"

#include <stdlib.h>

struct tree_node44 {
    struct tree_node44 *l;
    struct tree_node44 *r;
    int data;
    int count;
};

#define TREE_NODE44_COUNT(p) ((p) ? (p)->count : 0)
#define TREE_ACCUMLATE(p) (TREE_NODE44_COUNT((p)->l) + TREE_NODE44_COUNT((p)->r) + 1)

inline static struct tree_node44 *alloc_tree_node44(int data) {
    struct tree_node44 *node = (struct tree_node44*) malloc(sizeof(struct tree_node44));
    node->l = node->r = NULL;
    node->data = data;
    node->count = 1;
    return node;
}

static struct tree_node44 *find_min(struct tree_node44 *node) {
    if (node) {
        while (node->l) {
            node = node->l;
        }
    }
    return node;
}

struct tree_node44 *insert_tree_node44(struct tree_node44 *node, int data) {
    if (node) {
        if (data < node->data) {
            node->l = insert_tree_node44(node->l, data);
            node->count = TREE_ACCUMLATE(node);
        } else if(node->data < data) {
            node->r = insert_tree_node44(node->r, data);
            node->count = TREE_ACCUMLATE(node);
        }
    } else {
        node = alloc_tree_node44(data);
    }
    return node;
}

struct tree_node44 *delete_tree_node44(struct tree_node44 *node, int data) {
    if (!node) {
        return node;
    }
    if (data < node->data) {
        node->l = delete_tree_node44(node->l, data);
    } else if (node->data < data) {
        node->r = delete_tree_node44(node->r, data);
    } else if (node->l && node->r) {
        struct tree_node44 *tmp;
        tmp = find_min(node->r);
        node->data = tmp->data;
        node->r = delete_tree_node44(node->r, tmp->data);
    } else {
        struct tree_node44 *tmp = node;
        node = node->l ? node->l : node->r;
        free(tmp);
    }
    node->count = TREE_ACCUMLATE(node);
    return node;
}

struct tree_node44 *tree_node44_find_kmin(struct tree_node44 *node, int k) {
    struct tree_node44 *ret = NULL;

    if (node && 0 <= k && k < node->count) {
        ret = tree_node44_find_kmin(node->l, k);
        if (ret == NULL) {
            ret = tree_node44_find_kmin(node->r, k - TREE_NODE44_COUNT(node->l));
        }
        if (ret == NULL) {
            ret = node;
        }
    }
    return ret;
}

