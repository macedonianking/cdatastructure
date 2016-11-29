#include "chapter5/hash_table_rehash.h"

#include <stdlib.h>

#include "macros.h"
#include "util.h"
#include "log.h"

struct entry {
    int key;
    int value;
    int state;
};

struct hash_table {
    struct entry *queue;
    int c;
};

#define STATE_EMPTY     0
#define STATE_INUSE     1
#define STATE_ERASE     2

static int is_prime(int n) {
    DCHECK(n >= 2);
    if (n == 2) {
        return 1;
    }
    int r = 1;
    for (int i = 3; i < n; ++i) {
        if (n % i == 0) {
            r = 0;
            break;
        }
    }
    return r;
}

static int get_least_prime(int n) {
    while (!is_prime(n)) {
        n++;
    }
    return n;
}

static void init_hash_table(struct hash_table *table, int c) {
    struct entry *entry;

    c = get_least_prime(c);
    table->queue = (struct entry*) malloc(c * sizeof(struct entry));
    if (!table->queue)
        fatal_error("init_hash_table malloc failure");
    for (entry = table->queue; entry < table->queue + c; ++entry) {
        entry->key = 0;
        entry->value = 0;
        entry->state = STATE_EMPTY;
    }
}

static struct entry *find_hash_table(struct hash_table *table, int key) {
    int hash = key % table->c;
    int i = 0, j;
    struct entry *ptr, *entry;

    entry = NULL;
    for (;;) {
        j = (hash + i * i) % table->c;
        i++;
        ptr = table->queue + j;
        if (ptr->state == STATE_EMPTY || ptr->state == STATE_ERASE) {
            entry = ptr;
            break;
        }
        else if(entry->key == key) {
            entry = ptr;
            break;   
        } else if ((entry->key % table->c) != hash) {
            break;
        }
    }

    return entry;
}

static void insert_hash_table(struct hash_table *table, int key, int value);

static void rehash_table(struct hash_table *table, int c) {
    struct hash_table temp_table;
    struct entry *ptr;

    init_hash_table(&temp_table, c);
    ptr = table->queue;
    for (int i = 0; i < table->c; ++i) {
        if (ptr->state == STATE_INUSE) {
            insert_hash_table(&temp_table, ptr->key, ptr->value);
        }
        ++ptr;
    }
    ptr = table->queue;
    table->queue = temp_table.queue;
    table->c = temp_table.c;
    free(ptr);
}

void insert_hash_table(struct hash_table *table, int key, int v) {
    struct entry *entry;

    entry = find_hash_table(table, key);
    if (entry) {
        entry->key = key;
        entry->value = v;
        entry->state = STATE_INUSE;
        return;
    }

    rehash_table(table, 2 * (table->c + 1));
    entry = find_hash_table(table, key);
    DCHECK(entry);
    entry->key = key;
    entry->value = v;
    entry->state = STATE_INUSE;
}

static void hash_table_remove(struct hash_table *table, int key) {
    struct entry *entry;

    entry = find_hash_table(table, key);
    if (entry != NULL && entry->state == STATE_INUSE) {
        entry->state = STATE_ERASE;
    }
}

static void free_hash_table(struct hash_table *table) {
    free(table->queue);
    table->queue = NULL;
    table->c = 0;
}

void chapter5_5_tutorial() {
    struct hash_table table;
    int data_buf[] = {4371, 1323, 6173, 4199, 4344, 9679, 1989};

    init_hash_table(&table, 7);
    for (int i = 0; i < ARRAY_SIZE(data_buf); ++i) {
        insert_hash_table(&table, data_buf[i], 0);
    }
    hash_table_remove(&table, 4371);
    free_hash_table(&table);
}