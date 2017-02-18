#include "rb_tree.h"

#include <stdlib.h>

#include "macros.h"

static inline struct rb_node *grantparent(struct rb_node *node) {
    return node->parent->parent;
}

static inline struct rb_node *uncle(struct rb_node *node) {
    if (node->parent == grantparent(node)->left) {
        return grantparent(node)->right;
    } else {
        return grantparent(node)->left;
    }
}

static inline void rb_transplant(struct rb_tree *tree, struct rb_node *src,
        struct rb_node *dst) {
    if (src->parent == &tree->nil) {
        tree->root = dst;
    } else if (src->parent->left == src) {
        src->parent->left = dst;
    } else {
        src->parent->right = dst;
    }
    dst->parent = src->parent;
}

/**
 * 初始化一颗红黑树
 */
void init_rb_tree(struct rb_tree *tree) {
    tree->root = &tree->nil;
    tree->nil.left = tree->nil.right = tree->nil.parent = &tree->nil;
    tree->nil.color = BLACK;
    tree->nil.key = -1;
}

/**
 * 生成一个红黑树的节点
 */
static inline struct rb_node *alloc_rb_node(int key) {
    struct rb_node *ptr = (struct rb_node*) malloc(sizeof(*ptr));
    DCHECK(ptr);
    ptr->left = ptr->right = ptr->parent = NULL;
    ptr->key = key;
    ptr->color = BLACK;
    return ptr;
}

static inline void free_rb_node(struct rb_node *node) {
    free(node);
}

struct rb_node *rb_search(struct rb_tree *tree, int key) {
    struct rb_node *node;

    node = tree->root;
    while (node != &tree->nil && node->key != key) {
        if (key < node->key) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return node != &tree->nil ? node : NULL;
}

struct rb_node *rb_tree_minimum(struct rb_tree *tree, struct rb_node *node) {
    if (node != &tree->nil) {
        while (node->left != &tree->nil) {
            node = node->left;
        }
    }
    return node != &tree->nil ? node : NULL;
}

struct rb_node *rb_tree_maximum(struct rb_tree *tree, struct rb_node *node) {
    if (node != &tree->nil) {
        while (node->right != &tree->nil) {
            node = node->right;
        }
    }
    return node != &tree->nil ? node : NULL;
}

static void free_rb_tree_internal(struct rb_tree *tree, struct rb_node *node, 
        void (*callback)(struct rb_tree *tree, struct rb_node*, void *), void *data) {
    if (node != &tree->nil) {
        free_rb_tree_internal(tree, node->left, callback, data);
        free_rb_tree_internal(tree, node->right, callback, data);
        if (callback) {
            callback(tree, node, data);
        }
    }
}

void free_rb_tree(struct rb_tree *tree, void (*callback)(struct rb_tree *tree, struct rb_node*, void *), void *data) {
    if (tree->root != &tree->nil) {
        free_rb_tree_internal(tree, tree->root, callback, data);
    }
    tree->root = &tree->nil;
}

/**
 * node节点下降成node右子节点的做节点->左旋
 */
static void rb_rotate_left(struct rb_tree *tree, struct rb_node *node) {
    struct rb_node *right;

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
static void rb_rotate_right(struct rb_tree *tree, struct rb_node *node) {
    struct rb_node *left;
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
static void rb_insert_fixup(struct rb_tree *tree, struct rb_node *node) {
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
void insert_rb_tree(struct rb_tree *tree, struct rb_node *new_node) {
    struct rb_node *p, *node;
    p = &tree->nil;
    node = tree->root;

    while (node != &tree->nil) {
        p = node;
        if (new_node->key == node->key) {
            DCHECK(0);
        } else if (new_node->key < node->key) {
            node = node->left;
        } else {
            node = node->right;
        }
    }

    node = new_node;
    node->parent = p;
    if (p == &tree->nil) {
        tree->root = node;
    } else if (node->key < p->key) {
        p->left = node;
    } else {
        p->right = node;
    }
    node->left = node->right = &tree->nil;
    node->color = RED;
    rb_insert_fixup(tree, node);
}

/**
 * 修正删除后红黑树的属性
 */
static void rb_delete_fixup(struct rb_tree *tree, struct rb_node *node) {
    struct rb_node *w;
    while (node != tree->root && node->color == BLACK) {
        if (node == node->parent->left) {
            w = node->parent->right;
            if (w->color == RED) {
                node->parent->color = RED;
                w->color = BLACK;
                rb_rotate_left(tree, node->parent);
            } else if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                node = node->parent;
            } else if (w->left->color == RED && w->right->color == BLACK) {
                w->left->color = BLACK;
                w->color = RED;
                rb_rotate_right(tree, w);
            } else {
                w->color = node->parent->color;
                node->parent->color = BLACK;
                w->right->color = BLACK;
                rb_rotate_left(tree, node->parent);
                node = tree->root;
            }
        } else {
            w = node->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                node->parent->color = RED;
                rb_rotate_right(tree, node->parent);
            } else if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                node = node->parent;
            } else if (w->right->color == RED && w->left->color == BLACK) {
                w->color = RED;
                w->right->color = BLACK;
                rb_rotate_left(tree, w);
            } else {
                w->color = node->parent->color;
                node->parent->color = BLACK;
                w->left->color = BLACK;
                rb_rotate_right(tree, node->parent);
                node = tree->root;
            }
        }
    }
    node->color = BLACK;
}

static inline void rb_replace(struct rb_tree *tree, struct rb_node *dst,
        struct rb_node *src) {
    rb_transplant(tree, dst, src);
    dst->left = src->left;
    if (dst->left != &tree->nil) {
        dst->left->parent = dst;
    }
    dst->right = src->right;
    if (dst->right != &tree->nil) {
        dst->right->parent = dst;
    }
}

/**
 * 删除节点
 */
void delete_rb_tree(struct rb_tree *tree, struct rb_node *node) {
    DCHECK(node);
    struct rb_node *t;

    if (node->left != &tree->nil && node->right != &tree->nil) {
        /**
         * 改为删除下个节点
         */
        t = rb_tree_minimum(tree, node->right);
        DCHECK(t);
        delete_rb_tree(tree, t);
        rb_replace(tree, node, t);
    } else {
        t = node->left;
        if (t == &tree->nil) {
            t = node->right;
        }
        rb_transplant(tree, node, t);
        if (node->color == BLACK) {
            /**
             * 被删除的节点是黑色的节点,才需要重新调整
             */
            rb_delete_fixup(tree, t);
        }
    }
}

struct rb_node *rb_tree_next(struct rb_tree *tree, struct rb_node *node) {
    struct rb_node *p;
    if (node->right != &tree->nil) {
        return rb_tree_maximum(tree, node->right);
    }
    p = node->parent;
    while (p != &tree->nil && p->right == node) {
        node = p;
        p = node->parent;
    }
    return p != &tree->nil ? p : NULL;
}

struct rb_node *rb_tree_prev(struct rb_tree *tree, struct rb_node *node) {
    struct rb_node *p;
    if (node->left != &tree->nil) {
        return rb_tree_maximum(tree, node->left);
    }
    p = node->parent;
    while (p != &tree->nil && p->left) {
        node = p;
        p = node->parent;
    }
    return p != &tree->nil ? p : NULL;
}
