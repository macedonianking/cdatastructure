#include "chapter14/chapter14_2.h"

#include "util.h"
#include "macros.h"

struct rb_int_node {
    struct rb_int_node *left, *right, *p;
    int low, high, max;
};

struct rb_int_tree {
    struct rb_int_node *root;
    struct rb_int_node nil;
};

/**
 * node 和[low, high]是否重叠
 * @return 0标示重叠, -1表示[low, high]在左边, 1表示[low, high]在右边
 */
static int int_overlap(struct rb_int_node *node, int low, int high) {
    if (high < node->low) {
        return -1;
    } else if (low > node->high) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * 查找区间树第一个重叠的节点
 */
static inline struct rb_int_node *
rb_int_tree_search(struct rb_int_tree *tree, int low, int high) {
    struct rb_int_node *node;

    node = tree->root;
    while (node != &tree->nil && int_overlap(node, low, high)) {
        if (node->left != &tree->nil && low <= node->left->max) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return node;
}

static inline void rb_int_left_rotate(struct rb_int_tree *tree,
                                      struct rb_int_node *node) {
    struct rb_int_node *right;

    right = node->right;
    DCHECK(right != &tree->nil);
    node->right = right->left;
    if (node->right != &tree->nil) {
        node->right->p = node;
    }
    right->p = node->p;
    if (node->p == &tree->nil) {
        tree->root  = right;
    } else if (node == node->p->left) {
        node->p->left = right;
    } else {
        node->p->right = right;
    }
    right->left = node;
    node->p = right;
    right->max = node->max;
    node->max = MAX(node->high, MAX(node->left->max, node->right->max));
}

static inline void rb_int_right_rotate(struct rb_int_tree *tree,
                                       struct rb_int_node *node) {
    struct rb_int_node *left;

    left = node->left;
    DCHECK(left != &tree->nil);
    node->left = left->right;
    if (node->left != &tree->nil) {
        node->left->p = node;
    }
    left->p = node->p;
    if (node->p == &tree->nil) {
        tree->root = left;
    } else if (node->p->left == node) {
        node->p->left = left;
    } else {
        node->p->right = left;
    }
    left->right = node;
    node->p = left;
    left->max = node->max;
    node->max = MAX(node->high, MAX(node->left->max, node->right->max));
}

void chapter14_2_main() {

}
