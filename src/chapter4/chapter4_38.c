#include "chapter4/chapter4_38.h"

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "list.h"
#include "config.h"

struct btree_node;

/**
 * M阶的b树，中间节点的数目在[2/3M, M]之间，
 * 根节点数目在[1,4/3M]之间，
 * 叶子节点的子节点数目在[2/3M, 4/3M]之间
 */
struct btree {
    int order;
    int low_limit;
    int high_limit;
    struct btree_node *root;
};

struct btree_data {
    struct list_head node;
    int data;
};

struct btree_node {
    struct list_head node;
    struct list_head list;
    int data;
    int n;
    int is_leaf;
};

static void release_btree_node_children(struct btree_node *node) {
    if (node->is_leaf) {
        struct btree_data *ptr;
        LIST_FOR_EACH_ENTRY_SAFE(ptr, &node->list, node) {
            list_del(&ptr->node);
            free(ptr);
        }
    } else {
        struct btree_node *ptr;
        LIST_FOR_EACH_ENTRY_SAFE(ptr, &node->list, node) {
            release_btree_node_children(ptr);
            list_del(&ptr->node);
            free(ptr);
        }
    }
    node->data = 0;
    node->n = 0;
}

static struct btree_data *alloc_btree_data(int n) {
    struct btree_data *data = (struct btree_data*) malloc(sizeof(*data));
    data->data = n;
    INIT_LIST_HEAD(&data->node);
    return data;
}

static struct btree_node *alloc_btree_node() {
    struct btree_node *node =(struct btree_node*) malloc(sizeof(*node));
    node->is_leaf = 0;
    node->data = 0;
    node->n = 0;
    INIT_LIST_HEAD(&node->list);
    INIT_LIST_HEAD(&node->node);
    return node;    
}

static int get_node_start_data(struct btree_node *node) {
    if (node->is_leaf) {
        return list_entry(node->list.next, struct btree_data, node)->data;
    } else {
        return list_entry(node->list.next, struct btree_node, node)->data;
    }
}

// divide node children into two part, and insert the second part to the silbing position.
static void divide_btree_node(struct btree_node *node, struct btree_node *parent,
        struct btree *tree) {
    struct btree_node *next;
    next = alloc_btree_node();
    next->is_leaf = node->is_leaf;
    next->n = node->n / 2;
    node->n -= next->n;
    list_move_range(list_item_at(node->list.next, node->n),
                    &node->list, &next->list);
    next->data = get_node_start_data(next);
    list_add(&next->node, &node->node);
    parent->n++;
}

// node is leaf btree_node object, we add data to it's children list in order.
static int btree_leaf_node_add_data(int data, struct btree_node *node) {
    struct list_head *next = &node->list;
    struct btree_data *ptr;

    LIST_FOR_EACH_ENTRY(ptr, &node->list, node) {
        if (ptr->data == data) {
            return 0;
        } else if(ptr->data > data) {
            next = &ptr->node;
        }
    }
    ptr = alloc_btree_data(data);
    list_add_tail(&ptr->node, next);
    return 1;
}

static struct btree_node *get_btree_insert_child(int data, struct btree_node *node) {
    struct btree_node *ptr;
    struct btree_node *curr;
    DCHECK(!list_empty(&node->list));
    curr = list_entry(&node->list.next, struct btree_node, node);
    LIST_FOR_EACH_ENTRY(ptr, &node->list, node) {
        if (ptr->data <= data) {
            curr = ptr;
        } else {
            break;
        }
    }
    return curr;
}

// node's child count is out of range, try to fix it.
static void adjust_child_elements(struct btree_node *node, struct btree_node *parent, 
        struct btree *tree) {
    struct btree_node *sibling;
    struct list_head *child;
    if (node->node.prev != &parent->list) {
        sibling = list_entry(node->node.prev, struct btree_node, node);
        if (sibling->n  < tree->high_limit) {
            child = node->list.next;
            list_del(child);
            node->n--;
            node->data = get_node_start_data(node);
            list_add_tail(child, &sibling->list);
            sibling->n++;
            return;
        }
    }

    if (node->node.next != &parent->list) {
        sibling = list_entry(node->node.next, struct btree_node, node);
        if (sibling->n < tree->high_limit) {
            child = node->list.prev;
            list_del(child);
            --node->n;
            list_add(child, &sibling->list);
            ++sibling->n;
            sibling->data = get_node_start_data(sibling);
            return;
        }
    }

    sibling = alloc_btree_node();
    sibling->is_leaf = node->is_leaf;
    sibling->n = node->n / 2;
    node->n -= sibling->n;
    list_move_range(list_item_at(node->list.next, node->n), &node->list, &sibling->list);
    sibling->data = get_node_start_data(sibling);
    parent->n++;
}

static int btree_add_impl(int data, struct btree_node *node, struct btree *tree) {
    int r = 0;
    if (node->is_leaf) {
        if (btree_leaf_node_add_data(data, node)) {
            r = 1;
            ++node->n;
            node->data = get_node_start_data(node);
        } 
    } else {
        struct btree_node *child = get_btree_insert_child(data, node);
        if (child->data != data) {
            r = btree_add_impl(data, child, tree);
            node->data = get_node_start_data(node);
            if (child->n > tree->high_limit) {
                adjust_child_elements(child, node, tree);
            }
        }
    }
    return r;
}

static void btree_init(struct btree *tree, int m) {
    DCHECK(m >= 3);
    tree->order = m;
    tree->low_limit = m * 2 / 3; // lower bound
    tree->high_limit = (m * 4 + 2) / 3; // upper bound
    tree->root = NULL;
}

static void btree_release(struct btree *tree) {
    if (tree->root) {
        release_btree_node_children(tree->root);
        free(tree->root);
        tree->root = NULL;
    }
}

static int btree_add(int data, struct btree *tree) {
    int r = 0;
    struct btree_node *node, *child;
    struct btree_data *dataPtr;

    node = tree->root;
    if (node == NULL) {
        node = alloc_btree_node();
        node->is_leaf = 1;
        tree->root = node;
        dataPtr = alloc_btree_data(data);
        list_add(&dataPtr->node, &node->list);
        node->data = data;
        node->n = 1;
        return 1;
    }

    if (node->is_leaf) {
        if (btree_leaf_node_add_data(data, node)) {
            node->data = get_node_start_data(node);
            node->n++;
            r = 1;
            goto check_count;
        }
        goto out;
    }

    child = get_btree_insert_child(data, node);
    if (child->data != data) {
        btree_add_impl(data, child, tree);
        node->data = get_node_start_data(node);
        if (child->n > tree->high_limit) {
            adjust_child_elements(child, node, tree);
        }
    }

check_count:
    if (node->n > tree->high_limit) {
        child = node;
        node = alloc_btree_node();
        tree->root = node;
        node->is_leaf = 0;
        list_add(&child->node, &node->list);
        node->data = get_node_start_data(node);
        ++node->n;
        divide_btree_node(child, node, tree);
    }
out:
    return r;
}

void chapter4_38_problem() {
    int data_buf[] = {3, 1, 4, 5, 9, 2, 6, 8, 7, 0};
    struct btree tree;

    btree_init(&tree, 3);
    for (int i = 0; i < ARRAY_SIZE(data_buf); ++i) {
        btree_add(data_buf[i], &tree);
    }
    btree_release(&tree);
}
