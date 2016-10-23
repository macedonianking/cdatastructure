#include "chapter4_8.h"

#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "macros.h"

struct ctree_node_t {
	struct ctree_node_t *left, *right;
	int data;
	int c;
};

static struct ctree_node_t *add_ctree_node_t(int data, int c, struct ctree_node_t *ptr) {
	if (!ptr) {
		ptr = (struct ctree_node_t*) malloc(sizeof(struct ctree_node_t));
		if (!ptr) {
			fatal_error("Error allocating memory %s %d", __FILE__, __LINE__);
		}
		ptr->data = data;
		ptr->c = c;
		ptr->left = ptr->right = NULL;
	} else {
		DCHECK(ptr->data != data);
		if (data < ptr->data) {
			ptr->left = add_ctree_node_t(data, c, ptr->left);
		} else {
			ptr->right = add_ctree_node_t(data, c, ptr->right);
		}
	}
	return ptr;
}

static void free_ctree_nodes(struct ctree_node_t *ptr) {
	if (ptr) {
		free_ctree_nodes(ptr->left);
		free_ctree_nodes(ptr->right);
		free(ptr);
	}
}

static void dump_ctree_nodes_preorder(struct ctree_node_t *ptr) {
	if (ptr) {
		fprintf(stdout, "%c ", ptr->c);
		dump_ctree_nodes_preorder(ptr->left);
		dump_ctree_nodes_preorder(ptr->right);
	}
}

static void dump_ctree_nodes_inorder(struct ctree_node_t *ptr) {
	if (ptr) {
		dump_ctree_nodes_inorder(ptr->left);
		fprintf(stdout, "%c ", ptr->c);
		dump_ctree_nodes_inorder(ptr->right);
	}
}

static void dump_ctree_nodes_postorder(struct ctree_node_t *ptr) {
	if (ptr) {
		dump_ctree_nodes_postorder(ptr->left);
		dump_ctree_nodes_postorder(ptr->right);
		fprintf(stdout, "%c ", ptr->c);
	}
}

void chapter4_8_problem() {
	int data[][2] = {
		{10, 'A'},
		{4, 'B'},
		{2, 'D'},
		{1, 'G'},
		{3, 'H'},
		{6, 'E'},
		{5, 'I'},
		{8, 'J'},
		{7, 'L'},
		{9, 'M'},
		{11, 'C'},
		{12, 'F'},
		{13, 'K'},
	};
	struct ctree_node_t *root;

	root = NULL;
	for (int i = 0; i < ARRAY_SIZE(data); ++i) {
		root = add_ctree_node_t(data[i][0], data[i][1], root);
	}

	dump_ctree_nodes_preorder(root);
	fputc('\n', stdout);

	dump_ctree_nodes_inorder(root);
	fputc('\n', stdout);

	dump_ctree_nodes_postorder(root);
	fputc('\n', stdout);

	free_ctree_nodes(root);
}
