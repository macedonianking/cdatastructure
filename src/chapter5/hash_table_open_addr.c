#include "chapter5/hash_table_open_addr.h"

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "util.h"

#define STATE_EMPTY     0
#define STATE_INUSE     1
#define STATE_DELETE    2

struct hash_node {
    int key;
    int value;
    int state;
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
        table->queue[i].state = STATE_EMPTY;
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
        if (node->state != STATE_INUSE) {
            key_ptr = node;
            break;
        }
    }
    return key_ptr;
}

/**
* 插入节点
*/
static struct hash_node *add_hash_table_linear(struct hash_table *table, int key) {
     struct hash_node *node;

    node = find_hash_node_linear(table, key);
    if (node) {
        node->key = key;
        node->state = STATE_INUSE;
    } else {
        fprintf(stderr, "insert_hash_node_linear failed for (%d)\n", key);
    }
    return node;
}

/**
* 使用线性函数解决冲突
*/
void chapter5_1_problem_b() {
    int data_buf[] = {4371, 1323, 6173, 4199, 4344, 9679, 1989};
    struct hash_table table;
    
    init_hash_table(&table, 31);
    for (int i = 0; i < NARRAY(data_buf); ++i) {
        add_hash_table_linear(&table, data_buf[i]);
    }
    free_hash_table(&table);
}

// 查找对应的节点是否找到
// 注意结束条件,quad 的hash方法必然找到节点或者找到空节点
static struct hash_node *find_hash_node_quad(struct hash_table *table, int v) {
    int hash = hash_int(table, v);
    int i, key_index;
    struct hash_node *node, *key_node;

    i = 0;
    key_node = NULL;
    for (;;) {
        key_index = i * i;
        ++i;
        node = table->queue + (key_index + hash) % table->count;
        if (node->state == STATE_EMPTY) {
            key_node = node;
            break;
        }
    }
    return key_node;
}

static void add_hash_table_quad(struct hash_table *table, int v) {
    struct hash_node *node;

    node = find_hash_node_quad(table, v);
    if (node->state == STATE_INUSE) {
        DCHECK(node->key == v);
    } else {
        node->key = v;
        node->state = STATE_INUSE;
    }
}

void chapter5_1_problem_c() {
    struct hash_table table;
    int data_buf[] = {4371, 1323, 6173, 4199, 4344, 9679, 1989};

    init_hash_table(&table, 31);
    for (int i = 0; i < NARRAY(data_buf); ++i) {
        add_hash_table_quad(&table, data_buf[i]);
    }
    free_hash_table(&table);
}

static struct hash_node *find_hash_node_double_hash(struct hash_table *table, int key) {
    int hash;
    int d;
    struct hash_node *node, *key_node;

    d = 7 - key % 7;
    hash = hash_int(table, key);
    key_node = NULL;
    while (!key_node) {
        node = table->queue + hash;
        if (node->state != STATE_INUSE) {
            key_node = node;
        } else {
            hash = (hash + d) % table->count;
        }
    }

    return key_node;
}

/**
 * 双hash的插入过程
 */
static void add_hash_table_double_hash(struct hash_table *table, int key) {
    struct hash_node *node;

    node = find_hash_node_double_hash(table, key);
    if (node->state != STATE_INUSE) {
        node->key = key;
        node->state = STATE_INUSE;
    }
}

void chapter5_1_problem_d() {
     struct hash_table table;
    int data_buf[] = {4371, 1323, 6173, 4199, 4344, 9679, 1989};

    init_hash_table(&table, 31);
    for (int i = 0; i < NARRAY(data_buf); ++i) {
        add_hash_table_double_hash(&table, data_buf[i]);
    }
    free_hash_table(&table); 
}