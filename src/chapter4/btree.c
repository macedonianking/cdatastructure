#include "chapter4/btree.h"

#include <stdlib.h>

#include "log.h"
#include "list.h"
#include "macros.h"

struct btree_node {
    struct list_head node;
    struct list_head list;
    int is_leaf;
    int n;
    int start;
};

struct btree_data_t {
    struct list_head node;
    int data;
};

struct btree {
    int order;
    int low;
    struct btree_node *root;
};

static struct btree_node *alloc_btree_node() {
    struct btree_node *ptr = (struct btree_node*) malloc(sizeof(struct btree_node));
    if (!ptr) {
        fatal_error("alloc_btree_node failure");
    }
    ptr->is_leaf = 0;
    ptr->n = 0;
    ptr->start = 0;
    INIT_LIST_HEAD(&ptr->node);
    INIT_LIST_HEAD(&ptr->list);
    return ptr;
}

static inline struct btree_data_t *alloc_btree_data(int data) {
    struct btree_data_t *ptr = (struct btree_data_t*) malloc(sizeof(*ptr));
    if(!ptr)
        fatal_error("alloc_btree_data.data=%d", data);
    INIT_LIST_HEAD(&ptr->node);
    ptr->data = data;
    return ptr;
}

/**
 * Get the first child node' value
 */
static inline int get_node_start_data(struct btree_node *ptr) {
    DCHECK(!list_empty(&ptr->list));
    int start;
    if (ptr->is_leaf) {
        start = list_entry(ptr->list.next, struct btree_data_t, node)->data;
    } else {
        start = list_entry(ptr->list.next, struct btree_node, node)->start;
    }
    return start;
}

static void init_btree(struct btree *tree, int order) {
    DCHECK(order >= 3);
    tree->order = order;
    tree->low = (order + 1) / 2;
    tree->root = NULL;
}

/**
 * 插入数据到节点
 * @param data 需要插入的数据
 * @param node 插入data数据到node的列表中
 * @return 如果插入成功，那么返回1，否则返回0
 */
static int add_btree_node_data(int data, struct btree_node *node) {
    struct btree_data_t *ptr, *next = NULL;
    LIST_FOR_EACH_ENTRY(ptr, &node->list, node) {
        if (ptr->data >= data) {
            next = ptr;
            break;
        }
    }
    if (next != NULL && next->data == data) {
        return 0;
    }
    ptr = alloc_btree_data(data);
    list_add_tail(&ptr->node, next != NULL ? &next->node : &node->list);
    return 1;
}

/**
 * 从ptr节点开始，跳过n个节点
 */
static struct list_head *list_skip_count(struct list_head *ptr, int n) {
    while (n-- > 0) {
        ptr = ptr->next;
    }
    return ptr;
}

/**
 * 把[start, end)的节点动到head的后方
 */
static void move_list_items(struct list_head *start, struct list_head *end, 
        struct list_head *head) {
    struct list_head *next;

    end = end->prev;
    __list_head_link(start->prev, end->next);
    next = head->next;
    __list_head_link(head, start);
    __list_head_link(end, next);
}

/**
 * 分裂node的children
 */
static void divide_btree_node(struct btree_node *parent, struct btree_node *node, 
        struct btree *tree) {
    DCHECK(node && node->n > tree->order);
    struct btree_node *next = alloc_btree_node();
    next->is_leaf = node->is_leaf;
    next->n = node->n / 2;
    node->n -= next->n;
    move_list_items(list_skip_count(node->list.next, node->n), &node->list, &next->list);
    next->start = get_node_start_data(next);
    list_add(&next->node, &node->node);
    ++parent->n;
}

/**
 * 在node的children查找插入的中间节点
 */
static struct btree_node *find_btree_node_add(int data, struct btree_node *node, struct btree *tree) {
    struct btree_node *ptr, *cur;
    DCHECK(!node->is_leaf);
    DCHECK(!list_empty(&node->list));

    cur = list_entry(node->list.next, struct btree_node, node);
    LIST_FOR_EACH_ENTRY(ptr, &node->list, node) {
        if (ptr->start <= data) {
            cur = ptr;
        } else {
            break;
        }
    }
    return cur;
}

static void btree_add_impl(int data, struct btree_node *node, struct btree *tree) {
    struct btree_node *child;
    if (node->is_leaf) {
        if (add_btree_node_data(data, node)) {
            ++node->n;
        }
    } else {
        child = find_btree_node_add(data, node, tree);
        if (child->start != data) {
            btree_add_impl(data, child, tree);
            if (child->n > tree->order) {
                divide_btree_node(node, child, tree);
            }
        }
    }
}

static void btree_add(int data, struct btree *tree) {
    struct btree_node *ptr = tree->root;
    struct btree_node *temp;
    struct btree_data_t *data_ptr;
    if (ptr == NULL) {
        ptr = alloc_btree_node();
        ptr->is_leaf = 1;
        ptr->start = data;
        data_ptr = alloc_btree_data(data);
        list_add(&data_ptr->node, &ptr->list);
        ++ptr->n;
        tree->root = ptr;
        return;
    }

    if (ptr->is_leaf) {
        /**
         * 根节点是叶子节点
         */
        if (add_btree_node_data(data, ptr)) {
            ++ptr->n;
            goto check_adjust_root;
        }
        goto out;
    } else {
        /**
         * 根节点是中间节点
         */
         temp = find_btree_node_add(data, ptr, tree);
         if (temp->start != data) {
             btree_add_impl(data, temp, tree);
             if (temp->n > tree->order) {
                 divide_btree_node(ptr, temp, tree);
             }
         }
    }

check_adjust_root:
    ptr->start = get_node_start_data(ptr);
    if (ptr->n > tree->order) {
        temp = alloc_btree_node();
        temp->is_leaf = 0;
        list_add(&ptr->node, &temp->list);
        temp->n = 1;
        temp->start = get_node_start_data(temp);
        divide_btree_node(temp, ptr, tree);
        tree->root = temp;
    }
out:
    return;
}

void chapter4_7_tutorial() {
    struct btree tree;
    init_btree(&tree, 3);
    btree_add(3, &tree);
    btree_add(2, &tree);
    btree_add(1, &tree);
    btree_add(5, &tree);
}