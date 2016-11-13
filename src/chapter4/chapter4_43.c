#include "chapter4/chapter4_43.h"

#include "chapter4/tree_node.h"
#include "macros.h"

static struct tree_node_t *transform(struct tree_node_t *node, int data) {
    DCHECK(node);
    if (data < node->data) {
        node->left = transform(node->left, data);
        node = tree_single_rotation_left(node);
    } else if (data > node->data) {
        node->right = transform(node->right, data);
        node = tree_single_rotation_right(node);
    }
    return node;
}

struct tree_node_t *transform_btree(struct tree_node_t *src, struct tree_node_t *dst) {
    if (src) {
        DCHECK(dst);
        src = transform(src, dst->data);
        src->left = transform_btree(src->left, dst->left);
        src->right = transform_btree(src->right, dst->right);
    }
    return src;
}