#include "chapter4/chapter4_39.h"

#include <stdlib.h>
#include <stdio.h>

#include "macros.h"
#include "list.h"
#include "log.h"

#define INVALID_MTREE_DATA  -1

struct mtree_node;
struct mtree {
    struct mtree_node *root;
};

struct mtree_node {
    struct list_head node;
    struct list_head list;
    int data;
};

static struct mtree_node *find_mtree_node(struct mtree_node *node, int data) {
    struct mtree_node *ptr, *ret;

    if (!node) {
        return NULL;
    }
    else if (node->data == data) {
        return node;
    } else {
        LIST_FOR_EACH_ENTRY(ptr, &node->list, node) {
            ret = find_mtree_node(ptr, data);
            if (ret != NULL) {
                return ret;
            }
        }
        return NULL;    
    }
}

static struct mtree_node *alloc_mtree_node(int data) {
    struct mtree_node *ptr = (struct mtree_node*) malloc(sizeof(struct mtree_node));
    if (!ptr)
        fatal_error("alloc_mtree_node fail");
    ptr->data = data;
    INIT_LIST_HEAD(&ptr->list);
    INIT_LIST_HEAD(&ptr->node);
    return ptr;
}

static struct mtree_node *insert_mtree_data(struct mtree *tree, int parent, int previous, 
        int data) {
    struct mtree_node *ptr, *item;
    item = NULL;
    if (previous != INVALID_MTREE_DATA) {
        ptr = find_mtree_node(tree->root, previous);
        if (ptr) {
            item = alloc_mtree_node(data);
            list_add(&item->node, &ptr->node);
        }
    } else if (parent != INVALID_MTREE_DATA) {
        ptr = find_mtree_node(tree->root, parent);
        if (ptr) {
            item = alloc_mtree_node(data);
            list_add(&item->node, &ptr->list);
        }
    }else {
        DCHECK(tree->root);
        item = alloc_mtree_node(data);
        tree->root = item;
    }
    return item;
}

static void release_mtree_node_children(struct mtree_node *node) {
    struct mtree_node *ptr;

    LIST_FOR_EACH_ENTRY_SAFE(ptr, &node->list, node) {
        release_mtree_node_children(ptr);
        list_del(&ptr->node);
        free(ptr);
    }
}

static void release_mtree(struct mtree *tree) {
    if(tree->root) {
        release_mtree_node_children(tree->root);
        free(tree->root);
        tree->root = NULL;
    }
}

static int dump_mtree_node(struct mtree_node *node, void *data) {
    fputc(node->data, stdout);
    fputc(' ', stdout);
    return 1;
}

static int traverse_mtree_inorder(struct mtree_node *node, 
        int (*handler)(struct mtree_node*, void*), void *data) {
    struct mtree_node *ptr;
    int r = 1;
    if (node) {
        r = handler(node, data);
        if(!r) {
            goto out;
        }

        LIST_FOR_EACH_ENTRY(ptr, &node->list, node) {
            r = traverse_mtree_inorder(ptr, handler, data);
            if (!r) {
                goto out;
            }
        }
    }
out:
    return r;
}

static int traverse_mtree_post_order(struct mtree_node *node, 
        int (*handler)(struct mtree_node *, void *), void *data) {
    struct mtree_node *ptr;
    int r = 1;
    if (node) {
        LIST_FOR_EACH_ENTRY(ptr, &node->list, node) {
            r = traverse_mtree_post_order(ptr, handler, data);
            if (!r) {
                goto out;
            }
        }
        r = handler(node, data);
    }

out:
    return r;
}

void chapter4_39_problem() {
    struct mtree tree;
    tree.root = NULL;
    insert_mtree_data(&tree, INVALID_MTREE_DATA, INVALID_MTREE_DATA, 'A');
    insert_mtree_data(&tree, 'A', INVALID_MTREE_DATA, 'B');
    insert_mtree_data(&tree, 'B', INVALID_MTREE_DATA, 'D');
    insert_mtree_data(&tree, 'D', INVALID_MTREE_DATA, 'H');
    insert_mtree_data(&tree, 'D', 'H', 'I');
    insert_mtree_data(&tree, 'B', 'D', 'E');
    insert_mtree_data(&tree, 'E', INVALID_MTREE_DATA, 'J');
    insert_mtree_data(&tree, 'A', 'B', 'C');
    insert_mtree_data(&tree, 'C', INVALID_MTREE_DATA, 'F');
    insert_mtree_data(&tree, 'F', INVALID_MTREE_DATA, 'K');
    insert_mtree_data(&tree, 'F', 'K', 'L');
    insert_mtree_data(&tree, 'L', INVALID_MTREE_DATA, 'O');
    insert_mtree_data(&tree, 'F', 'L', 'M');
    insert_mtree_data(&tree, 'M', INVALID_MTREE_DATA, 'P');
    insert_mtree_data(&tree, 'M', 'P', 'Q');
    insert_mtree_data(&tree, 'M', 'Q', 'R');
    insert_mtree_data(&tree, 'A', 'C', 'G');
    insert_mtree_data(&tree, 'G', INVALID_MTREE_DATA, 'N');
    traverse_mtree_inorder(tree.root, &dump_mtree_node, NULL);
    fputc('\n', stdout);
    traverse_mtree_post_order(tree.root, &dump_mtree_node, NULL);
    fputc('\n', stdout);
    release_mtree(&tree);
}