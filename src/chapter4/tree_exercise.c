#include "chapter4/tree_exercise.h"

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