#include "chapter12/btree.h"

#define LOG_TAG "btree"
#include "utils/apue.h"

typedef struct btree_node {
    struct btree_node *p, *l, *r;
    int key;
} btree_node;

typedef struct btree {
    struct btree_node *root;
} btree;

btree_node *btree_search(btree_node *ptr, int key) {
    while (ptr && ptr->key != key) {
        ptr = key < ptr->key ? ptr->l : ptr->r;
    }
    return ptr;
}

btree_node *btree_minimum(btree_node *ptr) {
    if (ptr) {
        while (ptr->l) {
            ptr = ptr->l;
        }
    }
    return ptr;
}

btree_node *btree_maximum(btree_node *ptr) {
    if (ptr) {
        while (ptr->r) {
            ptr = ptr->r;
        }
    }
    return ptr;
}

btree_node *btree_prev(btree_node *ptr) {
    if (!ptr) {
        return ptr;
    } else if (ptr->l) {
        return btree_maximum(ptr->l);
    } else {
        while (ptr->p && ptr->p->r != ptr)  {
            ptr = ptr->p;
        }
        return ptr->p;
    }
}

btree_node *btree_next(btree_node *ptr) {
    if (!ptr) {
        return ptr;
    } else if (ptr->r) {
        return btree_minimum(ptr->r);
    } else {
        while (ptr->p && ptr->p->l != ptr) {
            ptr = ptr->p;
        }
        return ptr->p;
    }
}

int btree_insert(btree *tree, btree_node *ptr) {
    btree_node *p, **iter;

    p = NULL;
    iter = &tree->root;
    while (*iter) {
        if (ptr->key == (*iter)->key) {
            return -1;
        } else if (ptr->key < (*iter)->key) {
            p = *iter;
            iter = &p->l;
        } else {
            p = *iter;
            iter = &p->r;
        }
    }

    *iter = ptr;
    ptr->p = p;
    return 0;
}

static btree_node **btree_pptr(btree *tree, btree_node *ptr) {
    btree_node **r;
    if (!ptr->p) {
        r = &tree->root;
    } else if (ptr->p->l == ptr) {
        r = &ptr->p->l;
    } else {
        r = &ptr->p->r;
    }
    return r;
}

/**
 * src和dst都部位NULL
 */
static void btree_replace(btree *tree, btree_node *src, btree_node *dst) {
    *btree_pptr(tree, src) = dst;
    dst->p = src->p;
    dst->l = src->l;
    dst->r = src->r;
    if (dst->l) {
        dst->l->p = dst;
    }
    if (dst->r) {
        dst->r->p = dst;
    }
}

/**
 * 从tree删除ptr节点,删除后ptr->p, ptr->l, ptr->r都为NULL
 */
void btree_delete(btree *tree, btree_node *ptr) {
    btree_node *tmp;

    if (ptr->l) {
        tmp = btree_maximum(ptr->l);
        btree_delete(tree, tmp);
        btree_replace(tree, ptr, tmp);
    } else if (ptr->r) {
        tmp = btree_minimum(ptr->r);
        btree_delete(tree, tmp);
        btree_replace(tree, ptr, tmp);
    } else {
        *btree_pptr(tree, ptr) = NULL;
    }
    ptr->p = ptr->l = ptr->r = NULL;
}
