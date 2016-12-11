#include "chapter4/avl_tree.h"

#include <stdlib.h>
#include "macros.h"

#include "util.h"

struct avl_tree_node {
	struct avl_tree_node *left, *right;
	int data;
	int height;
};

static inline int avl_tree_height(struct avl_tree_node *ptr) {
	return ptr ? ptr->height : -1;
}

static inline int calc_avl_tree_height(struct avl_tree_node *ptr) {
	return MAX(avl_tree_height(ptr->left), avl_tree_height(ptr->right)) + 1;
}

static struct avl_tree_node *avl_single_rotation_left(struct avl_tree_node *k1) {
	struct avl_tree_node *k2;

	DCHECK(k1 && k1->left);
	k2 = k1->left;
	k1->left = k2->right;
	k2->right = k1;
	k1->height = calc_avl_tree_height(k1);
	k2->height = calc_avl_tree_height(k2);

	return k2;
}

static struct avl_tree_node *avl_single_rotation_right(struct avl_tree_node *k1) {
	struct avl_tree_node *k2;

	DCHECK(k1 && k1->right);
	k2 = k1->right;
	k1->right = k2->left;
	k2->left = k1;
	k1->height = calc_avl_tree_height(k1);
	k2->height = calc_avl_tree_height(k2);

	return k2;
}

static struct avl_tree_node *avl_double_rotation_left(struct avl_tree_node *ptr) {
	DCHECK(ptr && ptr->left && ptr->left->right);

	ptr->left = avl_single_rotation_right(ptr->left);
	ptr = avl_single_rotation_left(ptr);
	return ptr;
}

static struct avl_tree_node *avl_double_rotation_right(struct avl_tree_node *ptr) {
	DCHECK(ptr && ptr->right && ptr->right->left);

	ptr->right = avl_single_rotation_left(ptr->right);
	ptr = avl_single_rotation_right(ptr);
	return ptr;
}

static struct avl_tree_node *avl_tree_node_add(struct avl_tree_node *ptr, int data) {
	if (!ptr) {
		ptr = (struct avl_tree_node*) malloc(sizeof(struct avl_tree_node));
		ptr->data = data;
		ptr->height = 0;
		ptr->left = ptr->right = NULL;
	} else {
		DCHECK(ptr->data != data);
		if (data < ptr->data) {
			ptr->left = avl_tree_node_add(ptr->left, data);
			if (avl_tree_height(ptr->left) - avl_tree_height(ptr->right) == 2) {
				if (data < ptr->left->data) {
					ptr = avl_single_rotation_left(ptr);
				} else {
					ptr = avl_double_rotation_left(ptr);
				}
			}
		} else {
			ptr->right = avl_tree_node_add(ptr->right, data);
			if (avl_tree_height(ptr->right) - avl_tree_height(ptr->left) == 2) {
				if (data > ptr->right->data) {
					ptr = avl_single_rotation_right(ptr);
				} else {
					ptr = avl_double_rotation_right(ptr);
				}
			}
		}
		ptr->height = calc_avl_tree_height(ptr);
	}
	return ptr;
}

static void dump_avl_tree_preorder(struct avl_tree_node *ptr) {
	if (ptr) {
		fprintf(stdout, "%d(%d) ", ptr->data, ptr->height);
		dump_avl_tree_preorder(ptr->left);
		dump_avl_tree_preorder(ptr->right);
	}
}

static void free_avl_trees(struct avl_tree_node *ptr) {
	if (ptr) {
		free_avl_trees(ptr->left);
		free_avl_trees(ptr->right);
		free(ptr);
	}
}

static struct avl_tree_node *avl_tree_find_min(struct avl_tree_node *ptr) {
	if (ptr && ptr->left) {
		return avl_tree_find_min(ptr->left);
	}
	return ptr;
}

struct avl_tree_node *avl_tree_del(struct avl_tree_node *ptr, int data) {
	if (!ptr) {
		return ptr;
	}

	if (data < ptr->data) {
		ptr->left = avl_tree_del(ptr->left, data);
		ptr->height = calc_avl_tree_height(ptr);
		if (avl_tree_height(ptr->right) - avl_tree_height(ptr->left) == 2) {
			if (avl_tree_height(ptr->right->right) >= avl_tree_height(ptr->right->left)) {
				ptr = avl_single_rotation_right(ptr);
			} else {
				ptr = avl_double_rotation_right(ptr);
			}
		}
	} else if (data > ptr->data) {
		ptr->right = avl_tree_del(ptr->right, data);
		ptr->height = calc_avl_tree_height(ptr);
		if (avl_tree_height(ptr->left) - avl_tree_height(ptr->right) == 2) {
			if (avl_tree_height(ptr->left->left) >= avl_tree_height(ptr->left->right)) {
				ptr = avl_single_rotation_left(ptr);
			} else {
				ptr = avl_double_rotation_left(ptr);
			}
		}
	} else {
		struct avl_tree_node *tmp;
		if (ptr->left && ptr->right) {
			tmp = avl_tree_find_min(ptr->right);
			ptr->data = tmp->data;
			ptr->right = avl_tree_del(ptr->right, tmp->data);
			ptr->height = calc_avl_tree_height(ptr);
			if (avl_tree_height(ptr->left) - avl_tree_height(ptr->right) == 2) {
				if (avl_tree_height(ptr->left->left) >= avl_tree_height(ptr->left->right)) {
					ptr = avl_single_rotation_left(ptr);
				} else {
					ptr = avl_double_rotation_left(ptr);
				}
			}
		} else {
			tmp = ptr;
			ptr = ptr->left ? ptr->left : ptr->right;
			free(tmp);
		}
	}
	return ptr;
}


void chapter4_4_avl_problem() {
	int data_buf[] = {3, 2, 1};
	struct avl_tree_node *root;

	root = NULL;
	for (int i = 0; i < NARRAY(data_buf); ++i) {
		root = avl_tree_node_add(root, data_buf[i]);
	}
	dump_avl_tree_preorder(root);
	free_avl_trees(root);
	root = NULL;
}

void chapter4_16_avl_problem() {
	int data_buf[] = {2, 1, 4, 5, 9, 3, 6, 7};
	struct avl_tree_node *root;

	root = NULL;
	for (int i = 0; i < NARRAY(data_buf); ++i) {
		root = avl_tree_node_add(root, data_buf[i]);
	}
	dump_avl_tree_preorder(root);
	fputc('\n', stdout);
	free_avl_trees(root);
	root = NULL;
}

struct avl_tree_node *chapter4_22_avl_double_rotation_left(struct avl_tree_node *k1) {
	struct avl_tree_node *k2, *k3;
	DCHECK(k1 && k1->left && k1->left->right);
	k2 = k1->left;
	k3 = k2->right;
	k1->left = k3->right;
	k2->right = k3->left;
	k1->height = calc_avl_tree_height(k1);
	k2->height = calc_avl_tree_height(k2);
	k3->left = k2;
	k3->right = k1;
	k3->height = calc_avl_tree_height(k3);
	return k3;
}

struct avl_tree_node *chapter4_22_avl_double_rotation_right(struct avl_tree_node *k1) {
	struct avl_tree_node *k2, *k3;
	DCHECK(k1 && k1->right && k1->right->left);
	k2 = k1->right;
	k3 = k2->left;
	k1->right = k3->left;
	k2->left = k3->right;
	k1->height = calc_avl_tree_height(k1);
	k2->height = calc_avl_tree_height(k2);
	k3->left = k1;
	k3->right = k2;
	k3->height = calc_avl_tree_height(k3);
	return k3;
}
