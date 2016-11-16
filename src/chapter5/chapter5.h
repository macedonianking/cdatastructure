#ifndef MAIN_CHPATER5_CHAPTER5_H
#define MAIN_CHPATER5_CHAPTER5_H

struct hash_table {
    struct list_head *queue;
    int count;
};

void init_hash_table(struct hash_table *table, int count);
void free_hash_table(struct hash_table *table);

void chapter5_2_tutorial();

void chapter5_1_problem_a();

#endif