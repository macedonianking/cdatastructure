#include "chapter13/daemon_process.h"

#define LOG_TAG "rbtree"
#include "utils/slog.h"
#include "utils/apue.h"

#include "chapter13/rbtree.h"

struct my_node {
    rbtree_node node;
    int key;
};

static struct my_node *alloc_my_node(int key) {
    struct my_node *ptr;

    ptr = (struct my_node*) malloc(sizeof(*ptr));
    INIT_RBNODE(&ptr->node);
    ptr->key = key;
    return ptr;
}

static int my_node_cmp(rbtree_node *lhs, rbtree_node *rhs) {
    struct my_node *lhs_impl = rb_entry(lhs, struct my_node, node);
    struct my_node *rhs_impl = rb_entry(rhs, struct my_node, node);
    return lhs_impl->key - rhs_impl->key;
}

void daemon_process_main(int argc, char **argv) {
    int array[] = {41, 38, 31, 12, 19, 8};
    int delete_array[] = {8, 12, 19, 31, 38, 41};
    rbtree tree;
    rbtree_node *ptr;
    struct my_node *impl, item;

    rbtree_init(&tree, &my_node_cmp);
    for (int i = 0; i < NARRAY(array); ++i) {
        impl = alloc_my_node(array[i]);
        rbtree_insert(&tree, &impl->node);
    }

    for (ptr = rbtree_maximum(&tree, tree.root); ptr != NULL; ptr = rbtree_prev(&tree, ptr)) {
        impl = rb_entry(ptr, struct my_node, node);
        ALOGE("%d", impl->key);
    }

    for (int i = 0; i < NARRAY(delete_array); ++i) {
        item.key = delete_array[i];
        ptr = rbtree_search(&tree, tree.root, &item.node);
        if (ptr != NULL) {
            rbtree_delete(&tree, ptr);
            impl = rb_entry(ptr, struct my_node, node);
            free(impl);
        }
    }
}

