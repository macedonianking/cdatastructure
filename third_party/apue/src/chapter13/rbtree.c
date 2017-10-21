#include "chapter13/rbtree.h"

#define LOG_TAG "rbtree"
#include "utils/apue.h"

#define BALCK   0
#define RED     1

typedef struct rbtree_node {
    struct rbtree_node *p, *l, *r;
    int key;
    int color;
} rbtree_node;

typedef struct rbtree {
    struct rbtree_node *root, nil;
} rbtree;

rbtree_node *rbtree_search(rbtree_node *ptr, int key) {
    while (ptr && ptr->key != key) {
        ptr = key < ptr->key ? ptr->l : ptr->r;
    }
    return ptr;
}

rbtree_node *rbtree_minimum(rbtree_node *ptr) {
    if (ptr) {
        while (ptr->l) {
            ptr = ptr->l;
        }
    }
    return ptr;
}

rbtree_node *rbtree_maximum(rbtree_node *ptr) {
    if (ptr) {
        while (ptr->r) {
            ptr = ptr->r;
        }
    }
    return ptr;
}

rbtree_node *rbtree_prev(rbtree_node *ptr) {
    if (!ptr) {
        return ptr;
    } else if (ptr->l) {
        return rbtree_maximum(ptr->l);
    } else {
        while (ptr->p && ptr->p->r != ptr)  {
            ptr = ptr->p;
        }
        return ptr->p;
    }
}

rbtree_node *rbtree_next(rbtree_node *ptr) {
    if (!ptr) {
        return ptr;
    } else if (ptr->r) {
        return rbtree_minimum(ptr->r);
    } else {
        while (ptr->p && ptr->p->l != ptr) {
            ptr = ptr->p;
        }
        return ptr->p;
    }
}

static rbtree_node **rbtree_pptr(rbtree *tree, rbtree_node *ptr) {
    if (tree->root == ptr) {
        return &tree->root;
    } else {
        return ptr->p->l == ptr ? &ptr->p->l : &ptr->p->r;
    }
}

/**
 * 左旋, ptr下降成左节点
 */
void rbtree_rotate_left(rbtree *tree, rbtree_node *ptr) {
    rbtree_node *right;

    right = ptr->r;
    *rbtree_pptr(tree, ptr) = right;
    right->p = ptr->p;

    ptr->r = right->l;
    if (ptr->r != &tree->nil) {
        ptr->r->p = ptr;
    }

    right->l = ptr;
    ptr->p = right;
}

/**
 * 右旋, ptr下降成右节点
 */
void rbtree_rotate_right(rbtree *tree, rbtree_node *ptr) {
    rbtree_node *left;

    left = ptr->l;
    *rbtree_pptr(tree, ptr) = left;
    left->p = ptr->p;

    ptr->l = left->r;
    if (ptr->l != &tree->nil) {
        ptr->l->p = ptr;
    }

    left->r = ptr;
    ptr->p = left;
}

static rbtree_node *rbtree_sibling(rbtree_node *ptr) {
    if (ptr == ptr->p->l) {
        return ptr->p->r;
    } else {
        return ptr->p->l;
    }
}

/**
 * 红黑树插入后需要修复性质
 */
static void rbtree_insert_fixup(rbtree *tree, rbtree_node *ptr) {
    while (ptr->p->color == RED) {
        if (rbtree_sibling(ptr->p)->color == RED) {
            ptr->p->color = rbtree_sibling(ptr->p)->color = BALCK;
            ptr = ptr->p->p;
            ptr->color = RED;
        } else if (ptr->p == ptr->p->p->l) {
            if (ptr == ptr->p->r) {
                ptr = ptr->p;
                rbtree_rotate_left(tree, ptr);
            } else {
                ptr->p->p->color = RED;
                ptr->p->color = BALCK;
                rbtree_rotate_right(tree, ptr->p->p);
            }
        } else {
            if (ptr == ptr->p->l) {
                ptr = ptr->p;
                rbtree_rotate_right(tree, ptr);
            } else {
                ptr->p->p->color = RED;
                ptr->p->color = BALCK;
                rbtree_rotate_left(tree, ptr->p->p);
            }
        }
    }
}

int rbtree_insert(rbtree *tree, rbtree_node *ptr) {
    rbtree_node **iter, *p;

    p = &tree->nil;
    iter = &tree->root;
    while (*iter != &tree->nil) {
        if ((*iter)->key == ptr->key) {
            return -1;
        } else if (ptr->key < (*iter)->key) {
            p = *iter;
            iter = &(*iter)->l;
        } else {
            p = *iter;
            iter = &(*iter)->r;
        }
    }

    *iter = ptr;
    ptr->p = p;
    ptr->l = ptr->r = &tree->nil;
    ptr->color = RED;
    rbtree_insert_fixup(tree, ptr);
    return 0;
}
