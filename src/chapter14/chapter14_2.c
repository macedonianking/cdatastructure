#include "chapter14/chapter14_2.h"

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
        if (node->left != &tree->nil && low < node->left->max) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return node;
}

void chapter14_2_main() {

}
