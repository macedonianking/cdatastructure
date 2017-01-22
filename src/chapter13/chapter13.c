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

static void free_rb_tree(struct rb_node_t *node) {
    if (node != NULL) {
        free_rb_tree(node->left);
        free_rb_tree(node->right);
        free_rb_node(node);
    }
}

/**
 * k1节点左旋
 */
static void rb_rotate_left(struct rb_node_t **root, struct rb_node_t *k1) {
    DCHECK(k1->right);
    struct rb_node_t *k2;

    k2 = k1->right;
    k1->right = k2->left;
    if (k1->right != NULL) {
        k1->right->parent = k1;
    }
    k2->left = k1;
    k2->parent = k1->parent;
    if (k1->parent == NULL) {
        *root = k2;
    } else if (k1->parent->left == k1){
        k1->parent->left = k2;
    } else {
        k1->parent->right = k2;
    }
    k1->parent = k2;
}

/**
 * k1节点右旋
 */
static void rb_rotate_right(struct rb_node_t **root, struct rb_node_t *k1) {
    DCHECK(k1->left);
    struct rb_node_t *k2;

    k2 = k1->left;
    k1->left = k2->right;
    if (k1->left != NULL) {
        k1->left->parent = k1;
    }
    k2->right = k1;
    k2->parent = k1->parent;
    if (k1->parent == NULL) {
        *root = k2;
    } else if (k1->parent->left == k1) {
        k1->parent->left = k2;
    } else {
        k1->parent->right = k2;
    }
    k1->parent = k2;
}

static void rb_insert_fixup(struct rb_node_t **root, struct rb_node_t *node) {
    while (node->color == RED) {
        if (node->parent == NULL) {
            node->color = BLACK;
        } else if (node->parent->color == BLACK) {
            return;
        } else if (uncle(node) != NULL && uncle(node)->color == RED) {
            grantparent(node)->color = RED;
            node->parent->color = BLACK;
            uncle(node)->color = BLACK;
            node = grantparent(node);
        } else if (node->parent->right == node && grantparent(node)->left == node->parent) {
            rb_rotate_left(root, node->parent); // node->parent左旋
            node = node->left;
        } else if (node->parent->left == node && grantparent(node)->right == node->parent) {
            rb_rotate_right(root, node->parent); // node->parent右旋
            node = node->right;
        } else if (node->parent->left == node && grantparent(node)->left == node->parent) {
            node->parent->color = BLACK;
            grantparent(node)->color = RED;
            rb_rotate_right(root, grantparent(node)); // node->parent->parent右旋
            node = node->parent;
        } else {
            node->parent->color = BLACK;
            grantparent(node)->color = RED;
            rb_rotate_left(root, grantparent(node)); // node->parent->parent左旋
            node = node->parent;
        }
    }   
}

void rb_insert(struct rb_node_t **root, int key) {
    struct rb_node_t *parent;
    struct rb_node_t *node;

    parent = NULL;
    node = *root;
    while (node != NULL) {
        if (node->key == key) {
            return;
        }
        parent = node;
        if (key < node->key) {
            node = node->left;
        } else {
            node = node->right;
        }
    }

    node = alloc_rb_node(key);
    if (parent == NULL) {
        *root = node;
    } else if (key < parent->key) {
        parent->left = node;
    } else {
        parent->right = node;
    }
    node->color = RED;
    node->parent = parent;
    rb_insert_fixup(root, node);
}

void chapter13_main() {
    chapter13_3_2_problem();
}

void chapter13_3_2_problem() {
    int buf[] = {41, 38, 31, 12, 19, 8};
    struct rb_node_t *root;

    root = NULL;
    for (int i = 0; i < NARRAY(buf); ++i) {
        rb_insert(&root, buf[i]);
    }

    free_rb_tree(root);
    root = NULL;
}
