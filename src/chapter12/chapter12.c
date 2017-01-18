#include "chapter12/chapter12.h"

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "util.h"
#include "log.h"
#include "list.h"

struct tree_node_t {
	struct tree_node_t *left;
	struct tree_node_t *right;
	struct tree_node_t *parent;
	struct list_head node;
	int state;
	int key;
};

static struct tree_node_t *alloc_tree_node(int key) {
	struct tree_node_t *ptr = (struct tree_node_t *) malloc(sizeof(struct tree_node_t));
	DCHECK(ptr != 0);
	INIT_LIST_HEAD(&ptr->node);
	ptr->left = ptr->right = ptr->parent = 0;
	ptr->key = key;
	return ptr;
}

static void free_tree_node(struct tree_node_t *ptr) {
	if (ptr) {
		ptr->left = ptr->right = ptr->parent = 0;
		ptr->key = 0;
		free(ptr);
	}
}

static void inorder_tree_walk(struct tree_node_t *node) {
	if (node) {
		inorder_tree_walk(node->left);
		printf("%d ", node->key);
		inorder_tree_walk(node->right);
	}
}

static void preorder_tree_walk(struct tree_node_t *node) {
	if (node) {
		printf("%d ", node->key);
		preorder_tree_walk(node->left);
		preorder_tree_walk(node->right);
	}
}

static void postorder_tree_walk(struct tree_node_t *node) {
	if (node) {
		preorder_tree_walk(node->left);
		preorder_tree_walk(node->right);
		printf("%d ", node->key);
	}
}

static struct tree_node_t *insert_tree_node(struct tree_node_t *node, int key) {
	if (node != NULL) {
		if (key == node->key) {
			return node;
		} else if (key < node->key) {
			node->left = insert_tree_node(node->left, key);
			node->left->parent = node;
		} else {
			node->right = insert_tree_node(node->right, key);
			node->right->parent = node;
		}
	} else {
		node = alloc_tree_node(key);
	}
	return node;
}

static struct tree_node_t *init_binary_tree(int *buf, int n) {
	struct tree_node_t *root = 0;
	for (int i = 0; i < n; ++i) {
		root = insert_tree_node(root, buf[i]);
	}
	return root;
}

static void free_binary_trees(struct tree_node_t *node) {
	if (node != NULL) {
		free_binary_trees(node->left);
		free_binary_trees(node->right);
		free_tree_node(node);
	}
}

void chapter12_main() {
	chapter12_3_1_problem();
}

void chapter12_1_1_problem() {
	int buf[] = {10, 4, 1, 5, 17, 16, 21};
	struct tree_node_t *root;

	root = init_binary_tree(buf, NARRAY(buf));
	inorder_tree_walk(root);
	fputc('\n', stdout);
	free_binary_trees(root);
	root = NULL;
}

#define STATE_INIT 		0
#define STATE_LEFT		1
#define STATE_RIGHT		2

static void inorder_tree_walk_without_recursively_prepare(struct tree_node_t *node) {
	if (node) {
		node->state = STATE_INIT;
		inorder_tree_walk_without_recursively_prepare(node->left);
		inorder_tree_walk_without_recursively_prepare(node->right);
	}
}

static void inorder_tree_walk_without_recursively_a(struct tree_node_t *root) {
	struct list_head list;
	struct tree_node_t *node;

	INIT_LIST_HEAD(&list);
	node = root;
	inorder_tree_walk_without_recursively_prepare(root);

	while (node) {
		switch(node->state) {
			case STATE_INIT: {
				list_add(&node->node, &list);
				node->state = STATE_LEFT;
				node = node->left;
				break;
			}
			case STATE_LEFT: {
				fprintf(stdout, "%d ", node->key);
				list_add(&node->node, &list);
				node->state = STATE_RIGHT;
				node = node->right;
				break;
			}
			case STATE_RIGHT: {
				node = NULL;
				break;
			}
			default: {
				DCHECK_NOT_REACH();
			}
		}
		if (node == NULL && !list_empty(&list)) {
			node = list_entry(list.next, struct tree_node_t, node);
			list_del(&node->node);
		}
	}
	fputc('\n', stdout);
}

static void inorder_tree_walk_without_recursively_b(struct tree_node_t *root) {
	struct tree_node_t *node;

	inorder_tree_walk_without_recursively_prepare(root);
	node = root;

	while (node) {
		switch (node->state) {
			case STATE_INIT: {
				node->state = STATE_LEFT;
				if (node->left != NULL) {
					node = node->left;
				}
				break;
			}
			case STATE_LEFT: {
				fprintf(stdout, "%d ", node->key);
				node->state = STATE_RIGHT;
				if (node->right) {
					node = node->right;
				} else {
					node = node->parent;
				}
				break;
			}
			case STATE_RIGHT: {
				node = node->parent;
				break;
			}
			default: {
				DCHECK_NOT_REACH();
			}
		}
	}
	fputc('\n', stdout);
}

static struct tree_node_t *tree_search(struct tree_node_t *node, int key) {
	if (node) {
		if (node->key == key) {
			return node;
		} else if (node->key < key) {
			return tree_search(node->right, key);
		} else {
			return tree_search(node->left, key);
		}
	}
	return 0;
}

void chapter12_1_3_problem() {
	int buf[] = {10, 4, 1, 5, 17, 16, 21};
	struct tree_node_t *root;

	root = init_binary_tree(buf, NARRAY(buf));
	inorder_tree_walk_without_recursively_a(root);
	inorder_tree_walk_without_recursively_b(root);
	free_binary_trees(root);
	root = NULL;
}

void chapter12_1_4_problem() {
	int buf[] = {10, 4, 1, 5, 17, 16, 21};
	struct tree_node_t *root;

	root = init_binary_tree(buf, NARRAY(buf));
	preorder_tree_walk(root);
	postorder_tree_walk(root);
	free_binary_trees(root);
	root = NULL;
}

static struct tree_node_t *tree_minimum(struct tree_node_t *node) {
	if (node != 0) {
		while (node->left) {
			node = node->left;
		}
	}
	return node;
}

static struct tree_node_t *tree_maximum(struct tree_node_t *node) {
	if (node != NULL) {
		while (node->right) {
			node = node->right;
		}
	}
	return node;
}

static struct tree_node_t *tree_successor(struct tree_node_t *node) {
	struct tree_node_t *parent;

	if (node) {
		if (node->right) {
			return tree_minimum(node->right);
		}

		parent = node->parent;
		while (parent != NULL && parent->right == node) {
			node = parent;
			parent= node->parent;
		}
		return parent;
	}
	return NULL;
}

static struct tree_node_t *tree_predecessor(struct tree_node_t *node) {
	struct tree_node_t *parent;

	if (node) {
		if (node->left) {
			return tree_maximum(node->left);
		}

		parent = node->parent;
		while (parent != NULL && parent->left == node) {
			node = parent;
			parent = node->parent;
		}
		return parent;
	}
	return NULL;
}

void chapter12_2_tutorial() {
	int buf[] = {10, 4, 1, 5, 17, 16, 21};
	struct tree_node_t *root;
	struct tree_node_t *ptr, *curr;

	root = init_binary_tree(buf, NARRAY(buf));
	ptr = tree_search(root, 17);
	if (ptr != NULL) {
		fprintf(stdout, "%d\n", ptr->key);
	}
	ptr = tree_maximum(root);
	if (ptr != NULL) {
		curr = tree_predecessor(ptr);
		if (curr != NULL) {
			fprintf(stdout, "%d\n", curr->key);
		}
	}
	ptr = tree_minimum(root);
	if (ptr != NULL) {
		curr = tree_successor(ptr);
		fprintf(stdout, "%d\n", curr->key);
	}
	free_binary_trees(root);
	root = NULL;
}

static int judge_binary_tree_search(int *buf, int n, int key, int min, int max) {
	int i;
	DCHECK(min <= max);
	DCHECK(min <= key && key <= max);

	for (i = 0; i < n && buf[i] != key; ++i) {
		if (buf[i] <= min || buf[i] >= max) {
			break;
		}
		if (buf[i] < key) {
			min = buf[i];
		} else if (buf[i] > key) {
			max = buf[i];
		}
	}
	return i >= 0 && n > 0 && i == n - 1 && buf[i] == key;
}

static void judge_binary_tree_path(int *buf, int n, int ch) {
	if (!judge_binary_tree_search(buf, n, 363, 1, 1000)) {
		fprintf(stdout, "%c array failure\n", ch);
	}
}

void chapter12_2_1_problem() {
	int buf_a[] = {2, 252, 401, 398, 330, 344, 397, 363};
	int buf_b[] = {924, 220, 911, 244, 898, 258, 362, 363};
	int buf_c[] = {925, 202, 911, 240, 912, 245, 363};
	int buf_d[] = {2, 399, 387, 219, 266, 382, 381, 278, 363};
	int buf_e[] = {935, 278, 347, 621, 299, 392, 358, 363};

	judge_binary_tree_path(buf_a, NARRAY(buf_a), 'a');
	judge_binary_tree_path(buf_b, NARRAY(buf_b), 'b');
	judge_binary_tree_path(buf_c, NARRAY(buf_c), 'c');
	judge_binary_tree_path(buf_d, NARRAY(buf_d), 'd');
	judge_binary_tree_path(buf_e, NARRAY(buf_e), 'e');
}

#if 0
static struct tree_node_t *tree_maximum_recursively(struct tree_node_t *node) {
	if (node != NULL && node->right) {
		return tree_maximum_recursively(node->right);
	}
	return node;
}

static  struct tree_node_t *tree_minimum_recursively(struct tree_node_t *node) {
	if (node && node->left) {
		return tree_minimum_recursively(node->left);
	}
	return node;
}

#endif 

static void tree_insert_iterative(struct tree_node_t **root, int key) {
	struct tree_node_t *parent, *ptr;

	parent = NULL;
	ptr = *root;
	while (ptr != NULL && ptr->key != key) {
		parent = ptr;
		if (key < ptr->key) {
			ptr = ptr->left;
		} else {
			ptr = ptr->right;
		}
	}
	if (ptr != NULL && ptr->key == key) {
		return;
	}

	ptr = alloc_tree_node(key);
	ptr->parent = parent;
	if (parent == NULL) {
		*root = ptr;
	} else if (key < parent->key) {
		parent->left = ptr;
	} else {
		parent->right = ptr;
	}
}

static void tree_transplant(struct tree_node_t **root, 
							struct tree_node_t *k1, 
							struct tree_node_t *k2) {
	if (k1->parent == NULL) {
		*root = k2;
	} else if (k1 == k1->parent->left) {
		k1->parent->left = k2;
	} else {
		k1->parent->right = k2;
	}
	if (k2 != NULL) {
		k2->parent = k1->parent;
	}
}

static void tree_delete_iterative(struct tree_node_t **root, struct tree_node_t *k1) {
	DCHECK(k1);
	struct tree_node_t *k2;
	if (k1->left == NULL) {
		tree_transplant(root, k1, k1->right);
	} else if (k1->right) {
		tree_transplant(root, k1, k1->left);
	} else {
		k2 = tree_minimum(k1->right);
		if (k2 != k1->right) {
			tree_transplant(root, k2, k2->right);
			k2->right = k1->right;
			k2->right->parent = k2;
		}
		tree_transplant(root, k1, k2);
		k2->left = k1->left;
		k2->left->parent = k2;
	}
}

void chapter12_3_tutorial() {
	int buf[] = {10, 4, 1, 5, 17, 16, 21};
	struct tree_node_t *root;
	struct tree_node_t *k1;

	root = NULL;
	for (int i = 0; i < NARRAY(buf); ++i) {
		tree_insert_iterative(&root, buf[i]);
	}
	inorder_tree_walk(root);
	k1 = tree_search(root, 16);
	DCHECK(k1 != NULL);
	if (k1 != NULL) {
		tree_delete_iterative(&root, k1);
		free_tree_node(k1);
	}
	free_binary_trees(root);
	root = NULL;
}

static struct tree_node_t *tree_insert_recursive(struct tree_node_t *node, int key) {
	if (node == NULL) {
		node = alloc_tree_node(key);
	} else if (key < node->key) {
		node->left = tree_insert_recursive(node->left, key);
		node->left->parent = node;
	} else if (key > node->key) {
		node->right = tree_insert_recursive(node->right, key);
		node->right->parent = node;
	}
	return node;
}

/**
 * Insert BST recursively
 */
void chapter12_3_1_problem() {
	int buf[] = {10, 4, 1, 5, 17, 16, 21};
	struct tree_node_t *root;

	root = NULL;
	for (int i = 0; i < NARRAY(buf); ++i) {
		root = tree_insert_recursive(root, buf[i]);
	}
	inorder_tree_walk(root);
	fputc('\n', stdout);
	free_binary_trees(root);
	root = NULL;
}

static struct tree_node_t *tree_delete_recursive(struct tree_node_t *node, 
												 struct tree_node_t *k1) {
	if (node != k1) {
		if (k1->key < node->key) {
			node->left = tree_delete_recursive(node->left, k1);
			if (node->left != NULL) {
				node->left->parent = node;
			}
		} else {
			node->right = tree_delete_recursive(node->right, k1);
			if (node->right != NULL) {
				node->right->parent = node;
			}
		}
	} else {
		struct tree_node_t *k2;
		if (k1->left != NULL && k1->right != NULL) {
			k2 = tree_minimum(k1->right);
			DCHECK(k2);
			k1->right = tree_delete_recursive(k1->right, k2);
			k2->left = k1->left;
			k2->right = k1->right;
			if (k2->left != NULL) {
				k2->left->parent = k2;
			}
			if (k2->right != NULL) {
				k2->right->parent = k2;
			}
			node = k2;
		} else {
			k2 = k1->left;
			if (k2 == NULL) {
				k2 = k1->right;
			}
			node = k2;
		}
	}
	return node;
}

void chapter12_3_2_problem() {
	int buf[] = {10, 4, 1, 5, 17, 16, 21};
	struct tree_node_t *root;
	struct tree_node_t *k1;

	root = NULL;
	for (int i = 0; i < NARRAY(buf); ++i) {
		root = tree_insert_recursive(root, buf[i]);
	}
	k1 = tree_search(root, 17);
	if (k1 != NULL) {
		root = tree_delete_recursive(root, k1);
		free_tree_node(k1);
		k1 = NULL;
	}
	inorder_tree_walk(root);
	fputc('\n', stdout);
	free_binary_trees(root);
	root = NULL;
}
