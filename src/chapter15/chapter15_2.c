#include "chapter15/chapter15_2.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "macros.h"
#include "list.h"

struct matrix_t {
    double *mat;
    int rows, cols;
};

struct matrix_chain_node {
    struct list_head node;
    int start, end;
    int divide;
    int count;
};

struct matrix_chain_order_solution {
    struct list_head *queue;
    int n;
};

static inline void matrix_multiply(struct matrix_t *dst, struct matrix_t *lhs,
        struct matrix_t *rhs) {
    DCHECK(lhs->cols == rhs->rows);
    DCHECK(dst->rows == lhs->rows && dst->cols == rhs->cols);
    double v;
    double *rows, *cols;


    for (int r = 0; r != lhs->rows; ++r) {
        for (int c = 0; c != rhs->cols; ++c) {
            v = 0.0;
            rows = lhs->mat + lhs->rows * r;
            cols = rhs->mat + c;
            for (int k = 0; k != lhs->cols; ++k) {
                v += *rows * *cols;
                rows += 1;
                cols += rhs->rows; 
            }
            *(dst->mat + dst->rows * r + c) = v;
        }
    }
}

/**
 * 得到一个最优化子结构A[start, end]的节点
 */
static inline struct matrix_chain_node *alloc_matrix_chain_node(int start, int end, int divide) {
    struct matrix_chain_node *p = (struct matrix_chain_node*) malloc(sizeof(*p));
    DCHECK(p);
    INIT_LIST_HEAD(&p->node);
    p->start = start;
    p->end = end;
    p->divide = divide;
    p->count = 0;
    return p;
}

/**
 * 用key朝找对应的节点
 */
static struct matrix_chain_node *get_matrix_node_from_list_by_key(struct list_head *list,
        int key) {
    struct matrix_chain_node *ptr;
    struct matrix_chain_node *ret = NULL;
    LIST_FOR_EACH_ENTRY(ptr, list, node) {
        if (ptr->end == key) {
            ret = ptr;
        } else if (ptr->end > key) {
            break;
        }
    }
    return ret;
}

/**
 * 把node节点根据顺序插入到列表中
 */
static void add_matrix_node_to_list(struct list_head *list, struct matrix_chain_node *node) {
    struct matrix_chain_node *ptr;
    struct list_head *q = list;
    LIST_FOR_EACH_ENTRY(ptr, list, node) {
        if (ptr->end > node->end) {
            q = &ptr->node;
            break;
        }
    }
    // node 插入到q节点的前面
    list_add_tail(&node->node, q);
}

// 自底向上的求解矩阵连乘法的最优化结构
static void matrix_chain_order(int *matrix_dimens, int n, 
                        struct matrix_chain_order_solution *solution) {
    struct matrix_chain_node *node;
    int count;
    int min_count;
    int min_k;
    for (int i = 1; i <= n; ++i) {
        node = alloc_matrix_chain_node(i, i, i);
        list_add(&node->node, &solution->queue[i]);
    }

    for (int i = 2; i <= n; ++i) {
        for (int j = 1; (j + i -1) <= n; ++j) {
            min_count = INT_MAX;
            min_k = i;
            for (int k = i; k < j; ++k) {
                count = 0;
                node = get_matrix_node_from_list_by_key(&solution->queue[i], k); // 查找A[i, k]
                count += node->count;
                node = get_matrix_node_from_list_by_key(&solution->queue[k+1], j); // 查找A[k+1, j]
                count += node->count;
                count += matrix_dimens[i-1] * matrix_dimens[k] * matrix_dimens[j];
                if (min_count > count) {
                    min_count = count;
                    min_k = k;
                }
            }
            node = alloc_matrix_chain_node(i, j, min_k); // 插入A[i, j]最优化的子结构
            node->count = min_count;
            add_matrix_node_to_list(&solution->queue[i], node);
        }
    }
}

static void free_matrix_chain_node_list(struct list_head *list) {
    struct matrix_chain_node *ptr;
    LIST_FOR_EACH_ENTRY_SAFE(ptr, list, node) {
        free(ptr);
    }
}

static void init_matrix_chain_solution(struct matrix_chain_order_solution *solution, int n) {
    solution->n = n;
    solution->queue = (struct list_head *) malloc((n+1) * sizeof(*solution->queue));
    DCHECK(solution->queue);
    for (int i = 0; i <= solution->n; ++i) {
        INIT_LIST_HEAD(&solution->queue[i]);
    }
}

static void free_matrix_chain_solution(struct matrix_chain_order_solution *solution) {
    for (int i = 0; i <= solution->n; ++i) {
        free_matrix_chain_node_list(&solution->queue[i]);
    }
    free(solution->queue);
    solution->queue = NULL;
    solution->n = 0;
}

static void print_matrix_chain_order(struct matrix_chain_order_solution *solution, int start, int end) {
    struct matrix_chain_node *node;
    if (start == end) {
        fprintf(stdout, "A[%d]", start);
    } else {
        node = get_matrix_node_from_list_by_key(&solution->queue[start], end);
        DCHECK(node);
        fprintf(stdout, "(");
        print_matrix_chain_order(solution, start, node->divide);
        print_matrix_chain_order(solution, node->divide+1, end);
        fprintf(stdout, ")");
    }
}

void chapter15_2_1_problem() {
    int matrix_dimens[] = {5, 10, 3, 12, 5, 50, 6};
    int n;
    struct matrix_chain_order_solution solution;

    n = NARRAY(matrix_dimens)-1;
    init_matrix_chain_solution(&solution, n);
    matrix_chain_order(matrix_dimens, n, &solution);
    print_matrix_chain_order(&solution, 1, n);
    free_matrix_chain_solution(&solution);
}

void chapter15_2_main() {

}
