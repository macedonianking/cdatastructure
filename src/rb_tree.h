#ifndef rb_tree_h
#define rb_tree_h

#define BLACK   0
#define RED     1

#include "list.h"

struct rb_node {
    struct rb_node *left, *right, *parent;
    int color;
    int key;
};

struct rb_tree {
    struct rb_node *root;
    struct rb_node nil;
};

#define rb_entry(p, t, m) list_entry(p, t, m)

void init_rb_tree(struct rb_tree *tree);
void free_rb_tree(struct rb_tree *tree, void (*callback)(struct rb_tree *tree, struct rb_node*, void *), void *data);

/*
* 把node节点插入到tree中,tree中一定不能包含node->key的节点
*/
void insert_rb_tree(struct rb_tree *tree, struct rb_node *node);
/*
* 删除tree中的node节点,node节点必须在tree中
*/
void delete_rb_tree(struct rb_tree *tree, struct rb_node *node);

struct rb_node *rb_tree_next(struct rb_tree *tree, struct rb_node *node);
struct rb_node *rb_tree_prev(struct rb_tree *tree, struct rb_node *node);

struct rb_node *rb_tree_minimum(struct rb_tree *tree, struct rb_node *node);
struct rb_node *rb_tree_maximum(struct rb_tree *tree, struct rb_node *node);

struct rb_node *rb_search(struct rb_tree *tree, int key);

#endif /* rb_tree_h */
