#include "chapter5/chapter5.h"

#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "log.h"
#include "macros.h"
#include "util.h"

struct hash_node {
    struct list_head node;
    int key;
    int value;
};

struct hash_table {
    struct list_head *queue;
    int count;
};

static unsigned int hash_string(const char *str) {
    unsigned int hash = 0;
    while (*str != '\0') {
        hash = (hash << 5) - hash + *str++;    
    }
    
    return hash;
}

static unsigned int hash_int(int n) {
    return (unsigned int)((((unsigned int)n) << 5) - n);
}

void chapter5_2_tutorial() {
    unsigned int hash = hash_string("Hello world!!!");
    fprintf(stdout, "hash = %u\n", hash);
}

static struct hash_node *alloc_hash_node(int key) {
    struct hash_node *ptr = (struct hash_node*) malloc(sizeof(struct hash_node));
    DCHECK(ptr);
    if (!ptr) {
        fatal_error("alloc_hash_node");
    }
    INIT_LIST_HEAD(&ptr->node);
    ptr->key = key;
    ptr->value = 0;
    return ptr;
}

/**
 * 查找节点
 */
static struct hash_node *find_hash_node(struct hash_table *table, int key) {
    struct list_head *head;
    struct hash_node *node, *ptr;

    head = &table->queue[hash_int(key) % table->count];
    node = NULL;
    LIST_FOR_EACH_ENTRY(ptr, head, node) {
        if (ptr != NULL && ptr->key == key) {
            node = ptr;
            break;
        }
    }
    return node;
}

static void insert_hash_table(struct hash_table *table, int key, int value) {
    struct hash_node *node;

    node = find_hash_node(table, key);
    if (node != NULL) {
        node->value = value;
        return;
    }
    node = alloc_hash_node(key);
    node->value = value;
    list_add(&node->node, &table->queue[hash_int(key) % table->count]);
}

static void init_hash_table(struct hash_table *table, int count) {
    table->queue = (struct list_head*) malloc(sizeof(struct list_head) * count);
    if (!table->queue) {
        fatal_error("init_hash_table");
    }
    for (int i = 0; i < count; ++i) {
        INIT_LIST_HEAD(&table->queue[i]);
    }
    table->count = count;
}

static void free_hash_table(struct hash_table *table) {
    if (table) {
        free(table->queue);
    }
}

static void free_hash_table_chains(struct hash_table *table) {
    struct hash_node *node;
    struct list_head *head;
    
    for (int i = 0; i < table->count; ++i) {
        head = &table->queue[i];
        while (!list_empty(head)) {
            node = list_entry(head->next, struct hash_node, node);
            list_del(&node->node);
            free(node);
        }
    }
}

void chapter5_1_problem_a() {
    struct hash_table table;
    int data_buf[] = {4371, 1329, 6173, 4199, 4344, 9679, 1989};
    
    init_hash_table(&table, 10);
    for (int i = 0; i < NARRAY(data_buf); ++i) {
        insert_hash_table(&table, data_buf[i], 0);
    }

    free_hash_table_chains(&table);
    free_hash_table(&table);
}
