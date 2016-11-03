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

static int del_btree_node_data(int data, struct btree_node *node) {
    int r = 0;
    struct btree_data_t *ptr;
    LIST_FOR_EACH_ENTRY(ptr, &node->list, node) {
        if (ptr->data == data) {
            list_del(&ptr->node);
            free(ptr);
            r = 1;
            break;
        } else if (ptr->data > data) {
            break;
        }
    }
    return r;
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
static struct btree_node *find_btree_node_add(int data, struct btree_node *node, 
        struct btree *tree) {
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

static int btree_add_impl(int data, struct btree_node *node, struct btree *tree) {
    struct btree_node *child;
    int r = 0;
    if (node->is_leaf) {
        if (add_btree_node_data(data, node)) {
            r = 1;
            node->start = get_node_start_data(node);
            ++node->n;
        }
    } else {
        child = find_btree_node_add(data, node, tree);
        if (child->start != data) {
            r = btree_add_impl(data, child, tree);
            node->start = get_node_start_data(node);
            if (child->n > tree->order) {
                divide_btree_node(node, child, tree);
            }
        }
    }
    return r;
}

static int btree_add(int data, struct btree *tree) {
    struct btree_node *ptr = tree->root;
    struct btree_node *temp;
    struct btree_data_t *data_ptr;
    int r = 0;
    if (ptr == NULL) {
        ptr = alloc_btree_node();
        ptr->is_leaf = 1;
        ptr->start = data;
        data_ptr = alloc_btree_data(data);
        list_add(&data_ptr->node, &ptr->list);
        ++ptr->n;
        tree->root = ptr;
        r = 1;
        goto out;
    }

    if (ptr->is_leaf) {
        /**
         * 根节点是叶子节点
         */
        if (add_btree_node_data(data, ptr)) {
            r = 1;
            ptr->start = get_node_start_data(ptr);
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
             r = btree_add_impl(data, temp, tree);
             ptr->start = get_node_start_data(ptr);
             if (temp->n > tree->order) {
                 divide_btree_node(ptr, temp, tree);
             }
         }
    }

check_adjust_root:
    if (ptr->n > tree->order) {
        temp = ptr;
        ptr = alloc_btree_node();
        ptr->is_leaf = 0;
        list_add(&temp->node, &ptr->list);
        ptr->n = 1;
        ptr->start = get_node_start_data(ptr);
        divide_btree_node(ptr, temp, tree);
        tree->root = ptr;
    }
out:
    return r;
}

static void btree_take_or_combine_sibling(struct btree_node *node, 
    struct btree_node *parent, struct btree *tree) {
    struct btree_node *ptr;
    struct list_head *child;

    if (node->node.prev != &parent->list) {
        ptr = list_entry(node->node.prev, struct btree_node, node);
        if (ptr->n > tree->low) {
            // 可以从前面的节点移动一个子节点过来
            child = ptr->list.prev;
            list_del(child);
            --ptr->n;
            list_add(child, &node->list);
            ++node->n;
            node->start = get_node_start_data(node);
            return;
        } else if ((ptr->n + node->n) <= tree->order) {
            // 两个节点可以合并
            list_move(&ptr->list, &node->list);
            node->n += ptr->n;
            node->start = get_node_start_data(node);
            list_del(&ptr->node);
            --parent->n;
            free(ptr);
            return;
        }
    }

    if (node->node.next != &parent->list) {
        ptr = list_entry(node->node.next, struct btree_node, node);
        if (ptr->n > tree->low) {
            // 从后面的节点移动一个子节点过来
            child = ptr->list.next;
            list_del(child);
            --ptr->n;
            ptr->start = get_node_start_data(ptr);
            list_add_tail(child, &node->list);
            ++node->n;
            node->start = get_node_start_data(node);
            return;
        } else if ((ptr->n + node->n) <= tree->order) {
            // 合并本节点和后面的节点
            list_move_tail(&ptr->list, &node->list);
            node->n += ptr->n;
            ptr->n = 0;
            list_del(&ptr->node);
            parent->n--;
            free(ptr);
            return;
        }
    }
}

// FIXME:
static void btree_del_impl(int data, struct btree_node *node, struct btree *tree) {
    struct btree_node *child;
    if (node->is_leaf) {
        if (del_btree_node_data(data, node)) {
            --node->n;
            node->start = get_node_start_data(node);
        }
    } else {
        child = find_btree_node_add(data, node, tree);
        if (child->start <= data) {
            btree_del_impl(data, child, tree);
            node->start = get_node_start_data(node);
            if (child->n < tree->low) {
                btree_take_or_combine_sibling(child, node, tree);
            }
        }
    }
}

void btree_del(int data, struct btree *tree) {
    DCHECK(tree);
    struct btree_node *ptr, *temp;
    if (!tree->root || data < tree->root->start) {
        return;
    }

    ptr = tree->root;
    if (ptr->is_leaf) {
        /**
         * 根节点是叶节点
         */
        if (del_btree_node_data(data, ptr)) {
            if (!--ptr->n) {
                free(ptr);
                tree->root = NULL;
            } else {
                ptr->start = get_node_start_data(ptr);
            }
        }
        return;
    }

    temp = find_btree_node_add(data, ptr, tree);
    if (temp->start <= data) {
        btree_del_impl(data, temp, tree);
        ptr->start = get_node_start_data(ptr);
        if (temp->n < tree->low) {
            btree_take_or_combine_sibling(temp, ptr, tree);
        }
        if (ptr->n == 1) {
            temp = list_entry(ptr->list.next, struct btree_node, node);
            free(ptr);
            tree->root = temp;
        }
    }
    return;
}

void chapter4_7_tutorial() {
    int datas[] = {1, 8, 11, 12, 16, 17, 18, 19, 22, 23, 28, 31, 41, 52, 58, 59, 61};
    struct btree tree;
    init_btree(&tree, 3);
    for (int i = 0; i < ARRAY_SIZE(datas); ++i) {
        btree_add(datas[i], &tree);
    }
    for (int i = ARRAY_SIZE(datas) - 1; i >= 0; i--) {
        btree_del(datas[i], &tree);
    }
}