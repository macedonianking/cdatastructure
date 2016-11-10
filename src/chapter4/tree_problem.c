#include "chapter4/tree_problem.h"

#include <stdio.h>

#include "tree_node.h"

void chapter4_32_problem(struct tree_node_t *node, int k1, int k2) {
    if(node) {
        if (node->data > k1) {
            chapter4_32_problem(node->left, k1, k2);
        }
        if (k1 <= node->data && node->data <= k2) {
            fprintf(stdout, "%d ", node->data);
        }
        if (node->data < k2) {
            chapter4_32_problem(node->right, k1, k2);
        }
    }
}

static int is_tree_node_similar(struct tree_node_t *k1, struct tree_node_t *k2) {
    if (!k1 && !k2) {
        return 1;
    } else if (k1 && k2) {
        return is_tree_node_similar(k1->left, k2->left) 
            && is_tree_node_similar(k1->right, k2->right);
    } else {
        return 0;
    }
}

void chapter4_42_problem(struct tree_node_t *k1, struct tree_node_t *k2) {
    if (is_tree_node_similar(k1, k2)) {
        fputs("k1 and k2 is similar\n", stdout);
    } else {
        fputs("k1 and k2 is different\n", stdout);
    }
}

static int is_tree_node_iosmorphic(struct tree_node_t *k1, struct tree_node_t *k2) {
    if (!k1 && !k2) {
        return 0;
    } else if (k1 && k2 && k1->data == k2->data) {
        return (is_tree_node_iosmorphic(k1->left, k2->left) && is_tree_node_iosmorphic(k1->right, k2->right))
            || (is_tree_node_iosmorphic(k1->left, k2->right) && is_tree_node_iosmorphic(k1->right, k2->left));
           
    } else {
        return 0;
    }
}

void chapter4_43_problem(struct tree_node_t *k1, struct tree_node_t *k2) {
    int r = is_tree_node_iosmorphic(k1, k2);
    if (r) {
        fputs("k1 and k2 is iosmorphic\n", stdout);
    } else {
        fputs("k1 and k2 is not isomorphic\n", stdout);
    }
}