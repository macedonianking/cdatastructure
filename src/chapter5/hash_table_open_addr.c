#include "chapter5/hash_table_open_addr.h"

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "util.h"

struct hash_node {
    int key;
    int value;
    int in_use;
};

struct hash_table {
    struct hash_node *queue;
    int count;
};

static inline int hash_int(struct hash_table *table, int v) {
    return v % table->count;
}

static void init_hash_table(struct hash_table *table, int count) {
    table->queue = (struct hash_node*) malloc(sizeof(struct hash_node) * count);
    DCHECK(table->queue);
    table->count = count;
    for (int i = 0; i < count; ++i) {
        table->queue[i].key = 0;
        table->queue[i].value = 0;
        table->queue[i].in_use = 0;
    }
}

static void free_hash_table(struct hash_table *table) {
    free(table->queue);
    table->queue = NULL;
    table->count = 0;
}

/**
 * 查找key位v的hash_node
 */
static struct hash_node *find_hash_node_linear(struct hash_table *table, int v) {
    int hash = hash_int(table, v);
    struct hash_node *node, *key_ptr;

    key_ptr = NULL;
    for (int i = 0; i < table->count; ++i) {
        node = table->queue + (hash + i) % table->count;
        if (node->in_use && node->key == v) {
            key_ptr = node;
            break;
        }
    }
    return key_ptr;
}

/**
 * 插入hash节点
 */
static struct hash_node *insert_hash_node_linear(struct hash_table *table, int k) {
    int hash = hash_int(table, k);
    struct hash_node *key_ptr, *node;

    key_ptr = NULL;
    for (int i = 0; i < table->count; ++i) {
        node = table->queue + (hash + i) % table->count;
        if (!node->in_use) {
            key_ptr = node;
            break;
        }
    }
    if (key_ptr) {
        key_ptr->key = k;
        key_ptr->in_use = 1;
    }
    return key_ptr;
}

static void add_hash_table_linear(struct hash_table *table, int v) {
    struct hash_node *node = find_hash_node_linear(table, v);
    if (node == NULL) {
        node = insert_hash_node_linear(table, v);
        if (node == NULL) {
            fprintf(stderr, "MEET AN ERROR ON add_hash_table_linear for %d\n", v);
        }
    }
}

void chapter5_1_problem_b() {
    int data_buf[] = {4371, 1323, 6173, 4199, 4344, 9679, 1989};
    struct hash_table table;
    
    init_hash_table(&table, 31);
    for (int i = 0; i < ARRAY_SIZE(data_buf); ++i) {
        add_hash_table_linear(&table, data_buf[i]);
    }
    free_hash_table(&table);
}