#include "chapter13/rbtree.h"

#define LOG_TAG "rbtree"
#include "utils/apue.h"

#define BLACK   0
#define RED     1

rbtree_node *rbtree_search(rbtree *tree, rbtree_node *ptr, rbtree_node *key) {
    int r;
    while (ptr != &tree->nil && (r = tree->key_cmp(key, ptr))) {
        ptr = r < 0 ? ptr->l : ptr->r;
    }
    return ptr != &tree->nil ? ptr : NULL;
}

rbtree_node *rbtree_minimum(rbtree *tree, rbtree_node *ptr) {
    while (ptr->l != &tree->nil) {
        ptr = ptr->l;
    }
    return ptr;
}

rbtree_node *rbtree_maximum(rbtree *tree, rbtree_node *ptr) {
    while (ptr->r != &tree->nil) {
        ptr = ptr->r;
    }
    return ptr;
}

rbtree_node *rbtree_prev(rbtree *tree, rbtree_node *ptr) {
    if (ptr == &tree->nil) {
        return NULL;
    } else if (ptr->l != &tree->nil) {
        return rbtree_maximum(tree, ptr->l);
    } else {
        while (ptr->p != &tree->nil && ptr->p->r != ptr)  {
            ptr = ptr->p;
        }
        return ptr->p != &tree->nil ? ptr->p : NULL;
    }
}

rbtree_node *rbtree_next(rbtree *tree, rbtree_node *ptr) {
    if (ptr == &tree->nil) {
        return NULL;
    } else if (ptr->r != &tree->nil) {
        return rbtree_minimum(tree, ptr->r);
    } else {
        while (ptr->p != &tree->nil && ptr->p->l != ptr) {
            ptr = ptr->p;
        }
        return ptr->p != &tree->nil ? ptr->p : NULL;
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
            ptr->p->color = rbtree_sibling(ptr->p)->color = BLACK;
            ptr = ptr->p->p;
            ptr->color = RED;
        } else if (ptr->p == ptr->p->p->l) {
            if (ptr == ptr->p->r) {
                ptr = ptr->p;
                rbtree_rotate_left(tree, ptr);
            } else {
                ptr->p->p->color = RED;
                ptr->p->color = BLACK;
                rbtree_rotate_right(tree, ptr->p->p);
            }
        } else {
            if (ptr == ptr->p->l) {
                ptr = ptr->p;
                rbtree_rotate_right(tree, ptr);
            } else {
                ptr->p->p->color = RED;
                ptr->p->color = BLACK;
                rbtree_rotate_left(tree, ptr->p->p);
            }
        }
    }
    tree->root->color = BLACK;
}

int rbtree_insert(rbtree *tree, rbtree_node *ptr) {
    rbtree_node **iter, *p;
    int r;

    p = &tree->nil;
    iter = &tree->root;
    while (*iter != &tree->nil) {
        r = tree->key_cmp(ptr, (*iter));
        if (!r) {
            return -1;
        } else if (r < 0) {
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

static void rbtree_delete_fixup(rbtree *tree, rbtree_node *ptr) {
    rbtree_node *w;

    while (ptr != tree->root && ptr->color == BLACK) {
        if (ptr == ptr->p->l) {
            w = ptr->p->r;
            if (w->color == RED) {
                ptr->p->color = RED;
                w->color= BLACK;
                rbtree_rotate_left(tree, ptr->p);
            } else if (w->l->color == BLACK && w->r->color == BLACK) {
                w->color = RED;
                ptr = ptr->p;
            } else if (w->l->color == RED && w->r->color == BLACK) {
                w->l->color = BLACK;
                w->color = RED;
                rbtree_rotate_right(tree, w);
            } else {
                ptr->p->color = w->r->color = BLACK;
                rbtree_rotate_left(tree, ptr->p);
                ptr = tree->root;
                break; // finish
            }
        } else {
            w = ptr->p->l;
            if (w->color == RED) {
                ptr->p->color = RED;
                w->color = BLACK;
                rbtree_rotate_right(tree, ptr->p);
            } else if (w->l->color == BLACK && w->r->color == BLACK) {
                w->color = RED;
                ptr = ptr->p;
            } else if (w->l->color == BLACK && w->r->color == RED) {
                w->color = RED;
                w->r->color = BLACK;
                rbtree_rotate_left(tree, w);
            } else {
                w->l->color = ptr->p->color = BLACK;
                w->color = RED;
                rbtree_rotate_right(tree, ptr->p);
                ptr = tree->root;
                break; // finish
            }
        }
    }
    ptr->color = BLACK;
}

/**
 * dst不是在树中的节点
 */
static void rbtree_replace(rbtree *tree, rbtree_node *src, rbtree_node *dst) {
    *rbtree_pptr(tree, src) = dst;
    dst->p = src->p;
    dst->l = src->l;
    if (dst->l != &tree->nil) {
        dst->l->p = dst;
    }
    dst->r = src->r;
    if (dst->r != &tree->nil) {
        dst->r->p = dst;
    }
    dst->color = src->color;
}

/**
 * dst可以是src的子节点
 */
static void rbtree_transplant(rbtree *tree, rbtree_node *src, rbtree_node *dst) {
    *rbtree_pptr(tree, src) = dst;
    dst->p = src->p;
}

void rbtree_reset_node(rbtree_node *ptr) {
    ptr->p = ptr->l = ptr->r = NULL;
    ptr->color = RED;
}

void rbtree_delete(rbtree *tree, rbtree_node *ptr) {
    rbtree_node *tmp;
    int color;

    if (ptr->l != &tree->nil && ptr->r != &tree->nil) {
        tmp = rbtree_minimum(tree, ptr->r);
        rbtree_delete(tree, tmp);
        rbtree_replace(tree, ptr, tmp);
        rbtree_reset_node(ptr);
        return;
    }

    if (ptr->l != &tree->nil) {
        tmp = ptr->l;
    } else {
        tmp = ptr->r;
    }
    color = ptr->color;
    rbtree_transplant(tree, ptr, tmp);
    rbtree_reset_node(ptr);
    if (color == BLACK) {
        rbtree_delete_fixup(tree, tmp);
    }
}

void rbtree_init(rbtree *tree, int (*key_cmp)(rbtree_node *lhs, rbtree_node *rhs)) {
    tree->nil.color = BLACK;
    tree->nil.l = tree->nil.r = tree->nil.p = &tree->nil;
    tree->root= &tree->nil;
}
