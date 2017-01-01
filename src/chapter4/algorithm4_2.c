#include "chapter4/algorithm4_2.h"

#include <stdlib.h>

#include "macros.h"
#include "util.h"

struct square_matrix_t {
    double *matrix;
    int n;
};

/**
 * 设置成单位矩阵
 */
static void square_matrix_load_identity(struct square_matrix_t *matrix) {
    int n = matrix->n * matrix->n;
    double *ptr;

    for (int i = 0; i < n; ++i) {
        matrix->matrix[i] = 0.0;
    }
    ptr = matrix->matrix;
    for (int i = 0; i < matrix->n; ++i) {
        *ptr = 1.0;
        ptr += matrix->n + 1;
    }
}

static void init_square_matrix(struct square_matrix_t *matrix, int n) {
    matrix->matrix = (double*) malloc(n * n * sizeof(double));
    DCHECK(matrix->matrix);
    memset(matrix->matrix, 0, sizeof(n * n * sizeof(double)));
    matrix->n = n;
    square_matrix_load_identity(matrix);
}

static void free_square_matrix(struct square_matrix_t *matrix) {
    free(matrix->matrix);
    matrix->matrix = NULL;
}

static double square_matrix_multiply_row_col(struct square_matrix_t *lhs,
                                             struct square_matrix_t *rhs,
                                             int m, int n) {
    double sum = 0.0;
    double *lhs_ptr, *rhs_ptr;

    lhs_ptr = lhs->matrix + m * lhs->n;
    rhs_ptr = rhs->matrix + n;
    for (int i = 0; i < lhs->n; ++i) {
        sum += (*lhs_ptr) * (*rhs_ptr);
        ++lhs_ptr;
        rhs_ptr += rhs->n;
    }
    return sum;
}

static void square_matrix_multiply_simple(struct square_matrix_t *dst,
                                          struct square_matrix_t *lhs,
                                          struct square_matrix_t *rhs) {
    DCHECK(lhs->n == rhs->n && dst->n == lhs->n);
    double *dst_ptr;

    for (int m = 0; m < dst->n; ++m) {
        dst_ptr = dst->matrix + m * dst->n;
        for (int n = 0; n < dst->n; ++n) {
            *dst_ptr = square_matrix_multiply_row_col(lhs, dst, m, n);
            ++dst_ptr;        
        }
    }
}

static void dump_square_matrix(struct square_matrix_t *matrix) {
    double *ptr;

    for (int i = 0; i < matrix->n; ++i) {
        ptr = matrix->matrix + i * matrix->n;
        for (int j = 0; j < matrix->n; ++j) {
            fprintf(stdout, "%.2f\t\t", *ptr++);
        }
        fputc('\n', stdout);
    }
}

void algorithm4_2_tutorial() {
    struct square_matrix_t mat0;
    struct square_matrix_t mat1;
    struct square_matrix_t mat2;

    init_square_matrix(&mat0, 4);
    init_square_matrix(&mat1, 4);
    init_square_matrix(&mat2, 4);
    square_matrix_multiply_simple(&mat0, &mat1, &mat2);
    dump_square_matrix(&mat0);
    free_square_matrix(&mat0);
    free_square_matrix(&mat1);
    free_square_matrix(&mat2);
}