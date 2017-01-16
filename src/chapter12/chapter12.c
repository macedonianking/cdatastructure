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
	chapter12_1_3_problem();
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
