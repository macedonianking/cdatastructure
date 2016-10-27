#include "chapter4/splay_tree.h"

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "chapter4/tree_node.h"

static struct tree_node_t *splay_tree_zig_zig_left(struct tree_node_t *k1) {
	DCHECK(k1 && k1->left && k1->left->left);
	k1 = tree_single_rotation_left(k1);
	k1 = tree_single_rotation_left(k1);
	return k1;
}

static struct tree_node_t *splay_tree_zig_zig_right(struct tree_node_t *k1) {
	DCHECK(k1 && k1->right && k1->right->right);
	k1 = tree_single_rotation_right(k1);
	k1 = tree_single_rotation_right(k1);
	return k1;
}

static struct tree_node_t *splay_tree_access_impl(int data, struct tree_node_t *ptr, int *n) {
	if (!ptr) {
		return ptr;
	}

	if (data == ptr->data) {
		*n = 0;
		return ptr;
	}

	if (data < ptr->data) {
		ptr->left = splay_tree_access_impl(data, ptr->left, n);
		++*n;
		if (*n == 2) {
			if (data < ptr->left->data) {
				ptr = splay_tree_zig_zig_left(ptr);
			} else {
				ptr = tree_double_rotation_left(ptr);
			}
			*n = 0;
		}
 	} else {
		ptr->right = splay_tree_access_impl(data, ptr->right, n);
		++*n;
		if (*n == 2) {
			if (data > ptr->right->data) {
				ptr = splay_tree_zig_zig_right(ptr);
			} else {
				ptr = tree_double_rotation_right(ptr);
			}
			*n = 0;
		}
	} 
	return ptr;
}

static struct tree_node_t *splay_tree_access(int data, struct tree_node_t *ptr) {
	int n = -1;
	ptr = splay_tree_access_impl(data, ptr, &n);
	if (n == 1) {
		if (data < ptr->data) {
			ptr = tree_single_rotation_left(ptr);
		} else {
			ptr = tree_single_rotation_right(ptr);
		}
	}
	return ptr;
}

void chapter4_46_problem() {
	struct tree_node_t *root;

	root = NULL;
	for (int i = 7; i > 0; i--) {
		root = add_tree_node(i, root);
	}
	for (int i = 1; i <= 7; ++i) {
		root = splay_tree_access(i, root);
	}
	dump_tree_node_inorder(root);
	free_tree_nodes(root);
	root = NULL;
}
