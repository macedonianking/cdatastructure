#ifndef MAIN_CHAPTER4_TREE_NODE_H
#define MAIN_CHAPTER4_TREE_NODE_H

struct tree_node_t {
	struct tree_node_t *left, *right;
	int data;
};

struct tree_node_t *tree_single_rotation_left(struct tree_node_t *k1);
struct tree_node_t *tree_double_rotation_left(struct tree_node_t *k1);
struct tree_node_t *tree_single_rotation_right(struct tree_node_t *k1);
struct tree_node_t *tree_double_rotation_right(struct tree_node_t *k1);

struct tree_node_t *find_tree_node(int data, struct tree_node_t *ptr);
struct tree_node_t *find_tree_node_min(struct tree_node_t *ptr);
struct tree_node_t *find_tree_node_max(struct tree_node_t *ptr);
struct tree_node_t *add_tree_node(int data, struct tree_node_t *ptr);
struct tree_node_t *del_tree_node(int data, struct tree_node_t *ptr);
void free_tree_nodes(struct tree_node_t *ptr);
void dump_tree_node_inorder(struct tree_node_t *ptr);

#endif // MAIN_CHAPTER4_TREE_NODE_H
