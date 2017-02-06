#include "chapter13/chapter13.h"

#include <stdlib.h>

#include "util.h"
#include "macros.h"

#define BLACK   0
#define RED     1

struct rb_node_t {
    struct rb_node_t *left, *right, *parent;
    int color;
    int key;
};

struct rb_tree {
    struct rb_node_t *root;
    struct rb_node_t nil;
};

static inline struct rb_node_t *grantparent(struct rb_node_t *node) {
    return node->parent->parent;
}

static inline struct rb_node_t *uncle(struct rb_node_t *node) {
    if (node->parent == grantparent(node)->left) {
        return grantparent(node)->right;
    } else {
        return grantparent(node)->left;
    }
}

/**
 * 初始化一颗红黑树
 */
static inline void init_rb_tree(struct rb_tree *tree) {
    tree->root = &tree->nil;
    tree->nil.left = tree->nil.right = tree->nil.parent = &tree->nil;
    tree->nil.color = BLACK;
    tree->nil.key = -1;
}

/**
 * 生成一个红黑树的节点
 */
static inline struct rb_node_t *alloc_rb_node(int key) {
    struct rb_node_t *ptr = (struct rb_node_t*) malloc(sizeof(*ptr));
    DCHECK(ptr);
    ptr->left = ptr->right = ptr->parent = NULL;
    ptr->key = key;
    ptr->color = BLACK;
    return ptr;
}

static inline void free_rb_node(struct rb_node_t *node) {
    free(node);
}

static void free_rb_tree_internal(struct rb_tree *tree, struct rb_node_t *node) {
    if (node != &tree->nil) {
        free_rb_tree_internal(tree, node->left);
        free_rb_tree_internal(tree, node->right);
        free_rb_node(node);
    }
}

static void free_rb_tree(struct rb_tree *tree) {
    if (tree->root != &tree->nil) {
        free_rb_tree_internal(tree, tree->root);
    }
    tree->root = &tree->nil;
}

/**
 * node节点下降成node右子节点的做节点->左旋
 */
static void rb_rotate_left(struct rb_tree *tree, struct rb_node_t *node) {
    struct rb_node_t *right;

    DCHECK(node->right != &tree->nil);
    right = node->right;
    node->right = right->left;
    if (node->right != &tree->nil) {
        node->right->parent = node;
    }
    right->parent = node->parent;
    if (node->parent == &tree->nil) {
        tree->root = right;
    } else if (node->parent->left == node) {
        node->parent->left = right;
    } else {
        node->parent->right = right;
    }
    right->left = node;
    node->parent = right;
}

/**
 * node节点下降到左子节点的右节点位置->右旋
 */
static void rb_rotate_right(struct rb_tree *tree, struct rb_node_t *node) {
    struct rb_node_t *left;
    DCHECK(node->left);

    left = node->left;
    node->left = left->right;
    if (node->left != &tree->nil) {
        node->left->parent = node;
    }
    left->parent = node->parent;
    if (node->parent == &tree->nil) {
        tree->root = left;
    } else if (node->parent->left == node) {
        node->parent->left = left;
    } else {
        node->parent->right = left;
    }
    left->right = node;
    node->parent = left;
}

/**
 * 红黑树插入节点后属性修正
 */
static void rb_insert_fixup(struct rb_tree *tree, struct rb_node_t *node) {
    while (node->parent->color == RED) {
        if (node->parent == grantparent(node)->left) {
            if (uncle(node)->color == RED) {
                node->parent->color = BLACK;
                uncle(node)->color = BLACK;
                grantparent(node)->color = RED;
                node = grantparent(node);
            } else if (node == node->parent->right) {
                node = node->parent;
                rb_rotate_left(tree, node);
            } else {
                node->parent->color = BLACK;
                grantparent(node)->color = RED;
                rb_rotate_right(tree, grantparent(node));
            }
        } else {
            if (uncle(node)->color == RED) {
                uncle(node)->color = BLACK;
                node->parent->color = BLACK;
                grantparent(node)->color = RED;
                node = grantparent(node);
            } else if (node == node->parent->left) {
                node = node->parent;
                rb_rotate_right(tree, node);
            } else {
                node->parent->color = BLACK;
                grantparent(node)->color = RED;
                rb_rotate_left(tree, grantparent(node));
            }
        }
    }
    tree->root->color = BLACK;
}

/**
 * 红黑树的插入
 */
static void rb_insert(struct rb_tree *tree, int key) {
    struct rb_node_t *p, *node;
    p = &tree->nil;
    node = tree->root;

    while (node != &tree->nil) {
        p = node;
        if (key == node->key) {
            return;
        } else if (key < node->key) {
            node = node->left;
        } else {
            node = node->right;
        }
    }

    node = alloc_rb_node(key);
    node->parent = p;
    if (p == &tree->nil) {
        tree->root = node;
    } else if (key < p->key) {
        p->left = node;
    } else {
        p->right = node;
    }
    node->left = node->right = &tree->nil;
    node->color = RED;
    rb_insert_fixup(tree, node);
}

void chapter13_main() {
    chapter13_3_2_problem();
}

void chapter13_3_2_problem() {
    int buf[] = {41, 38, 31, 12, 19, 8};
    struct rb_tree tree;

    init_rb_tree(&tree);
    for (int i = 0; i < NARRAY(buf); ++i) {
        rb_insert(&tree, buf[i]);
    }

    free_rb_tree(&tree);
}
