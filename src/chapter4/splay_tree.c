#include "chapter4/splay_tree.h"

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "util.h"
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
		if(*n != -1) {
			++*n;
			if (*n == 2) {
				if (data < ptr->left->data) {
					ptr = splay_tree_zig_zig_left(ptr);
				} else {
					ptr = tree_double_rotation_left(ptr);
				}
				*n = 0;
			}
		}
 	} else {
		ptr->right = splay_tree_access_impl(data, ptr->right, n);
		if(*n != -1) {
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

struct tree_node_t *splay_tree_del(int data, struct tree_node_t *ptr) {
	ptr = splay_tree_access(data, ptr);
	if (ptr != NULL && ptr->data == data) {
		if (ptr->left) {
			struct tree_node_t *left = find_tree_node_max(ptr->left);
			left = splay_tree_access(left->data, ptr->left);
			left->right = ptr->right;
			ptr->left = ptr->right = NULL;
			free(ptr);
			ptr = left;
		} else {
			struct tree_node_t *right = ptr->right;
			ptr->right = NULL;
			free(ptr);
			ptr = right;
		}
	}
	return ptr;
}

void chapter4_46_tutorial() {
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

void chapter4_23_problem() {
	int buf[] = { 10, 4, 2, 1, 3, 6, 5, 8, 7, 9, 11, 12, 13 };
	struct tree_node_t *root = NULL;

	for (int i = 0; i < ARRAY_SIZE(buf); ++i) {
		root = add_tree_node(buf[i], root);
	}
	root = splay_tree_access(3, root);
	root = splay_tree_access(9, root);
	root = splay_tree_access(1, root);
	root = splay_tree_access(5, root);
	dump_tree_node_inorder(root);
	free_tree_nodes(root);
	root = NULL;
}
