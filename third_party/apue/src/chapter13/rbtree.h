#ifndef APUE_SRC_CHAPTER13_RBTREE_H
#define APUE_SRC_CHAPTER13_RBTREE_H

#include "utils/list.h"


typedef struct rbtree_node {
    struct rbtree_node *p, *l, *r;
    int color;
} rbtree_node;

typedef struct rbtree {
    struct rbtree_node *root, nil;
    int (*key_cmp)(rbtree_node *lhs, rbtree_node *rhs);
} rbtree;

#define rb_entry(ptr, type, m) CONTAINER_OF(ptr, type, m)
#define INIT_RBNODE(ptr) (ptr)->p = (ptr)->l = (ptr)->r = 0; (ptr)->color = 0

rbtree_node *rbtree_search(rbtree *tree, rbtree_node *ptr, rbtree_node *key);
rbtree_node *rbtree_minimum(rbtree *tree, rbtree_node *ptr);
rbtree_node *rbtree_maximum(rbtree *tree, rbtree_node *ptr);
rbtree_node *rbtree_prev(rbtree *tree, rbtree_node *ptr);
rbtree_node *rbtree_next(rbtree *tree, rbtree_node *ptr);
void rbtree_rotate_left(rbtree *tree, rbtree_node *ptr);
void rbtree_rotate_right(rbtree *tree, rbtree_node *ptr);
int  rbtree_insert(rbtree *tree, rbtree_node *ptr);
void rbtree_delete(rbtree *tree, rbtree_node *ptr);
void rbtree_reset_node(rbtree_node *ptr);
void rbtree_init(rbtree *tree, int (*key_cmp)(rbtree_node *lhs, rbtree_node *rhs));

#endif
