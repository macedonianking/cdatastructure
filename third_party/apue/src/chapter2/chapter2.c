#include "chapter2.h"

#include "utils/apue.h"

struct max_subarray_entity {
    int s;
    int e;
    int total;
};

int chapter2_main(int argc, char **argv) {
    return 0;
}

void my_insertion_sort(int *buf, int n) {
    int j;
    int key;

    for (int i = 1; i < n; ++i) {
        key = buf[i];
        for (j = i - 1; j >= 0 && buf[j] > key; --j) {
            buf[j+1] = buf[j];
        }
        buf[j+1] = key;
    }
}

void my_insertion_sort_reverse(int *buf, int n) {
    int key, j;

    for (int i = 1; i < n; ++i) {
        key = buf[i];
        for (j = i - 1; buf[j] < key; --j) {
            buf[j+1] = buf[j];
        }
        buf[j+1] = key;
    }
}

static void merge_impl(int *buf, int s, int m, int e, int *tmp) {
    int i, j, k;

    i = s;
    j = m+1;
    k = 0;
    while (i <=m && j <= e) {
        if (buf[i] <= buf[j]) {
            tmp[k++] = buf[i++];
        } else {
            tmp[k++] = buf[j++];
        }
    }
    while (i <= m) {
        tmp[k++] = buf[i++];
    }
    while (j <= e) {
        tmp[k++] = buf[j++];
    }
    for (i = s, j = 0; j < k;) {
        buf[i++] = buf[j++];
    }
}

static void merge_sort_impl(int *buf, int s, int e, int *tmp) {
    int m;

    if (s >= e) {
        return;
    }
    m = s + (e - s) / 2;
    merge_sort_impl(buf, s, m, tmp);
    merge_sort_impl(buf, m+1, e, tmp);
    merge_impl(buf, s, m, e, tmp);
}

void my_merge_sort(int *buf, int n) {
    int *tmp;

    if (n > 0) {
        tmp = (int*) malloc(sizeof(int) * n);
        merge_sort_impl(buf, 0, n-1, tmp);
        free(tmp);
    }
}

int my_binary_search(int *buf, int n, int key) {
    int s, e, m;
    int r;

    s = 0;
    e = n - 1;
    r = -1;
    while (s <= e) {
        m = s + (e - s) / 2;
        if (buf[m] == key) {
            r = m;
            break;
        } else if (key < buf[m]) {
            e = m - 1;
        } else {
            s = m + 1;
        }
    }

    return r;
}

void my_bubble_sort(int *buf, int n) {
    int max, j, tmp;

    for (int i = n-1; i >= 0; i--) {
        max = buf[0];
        for (j = 1; j <= i; ++i) {
            if (buf[j] > buf[max]) {
                max = j;
            }
        }
        tmp = buf[i];
        buf[i] = buf[max];
        buf[max] = tmp;
    }
}

void my_bubble_sort_reverse(int *buf, int n) {
    int min;
    int tmp;

    for (int i = n-1; i>=0; --i) {
        min = 0;
        for (int j = 1; j <= i; ++j) {
            if (buf[j] < buf[min]) {
                min = j;
            }
        }
        tmp = buf[i];
        buf[i] = buf[min];
        buf[min] = tmp;
    }
}

void my_more_bubble_sort(int *buf, int n) {
    int tmp;

    for (int i = n-1; i >= 0; --i) {
        for (int j = 1; j <= i; ++j) {
            if (buf[j] < buf[j-1]) {
                tmp = buf[j];
                buf[j] = buf[j-1];
                buf[j-1] = tmp;
            }
        }
    }
}

void my_more_bubble_sort_reverse(int *buf, int n) {
    int tmp;

    for (int i=n-1; i>=0; --i) {
        for (int j=1; j <= i; ++j) {
            if (buf[j] > buf[j-1]) {
                tmp = buf[j-1];
                buf[j-1] = buf[j];
                buf[j] = tmp;
            }
        }
    }
}

void compute_max_subarray_entity(int *buf, int s, int e, int m, struct max_subarray_entity *out) {
    int max, total;
    int max_s, max_e;

    max = buf[m];
    max_s = m;
    total = max;
    for (int i = m-1; i>= s; --i) {
        total += buf[i];
        if (total > max) {
            max = total;
            max_s = i;
        }
    }

    max_e = m;
    total = max;
    for (int i = m+1; i<=e; ++i) {
        total += buf[i];
        if (total > max) {
            max = total;
            max_e = i;
        }
    }

    out->s = max_s;
    out->e = max_e;
    out->total = max;
}

void max_subarray_entity_impl(int *buf, int s, int e, struct max_subarray_entity *out) {
    struct max_subarray_entity left, right, middle, *max;
    int m;

    if (s == e) {
        out->s = s;
        out->e = e;
        out->total = buf[s];
        return;
    }

    m = s + (e - s) / 2;
    max_subarray_entity_impl(buf, s, m, &left);
    max_subarray_entity_impl(buf, m+1, e, &right);
    compute_max_subarray_entity(buf, s, e, m, &middle);
    max = right.total > left.total ? &right : &left;
    if (middle.total > max->total) {
        max = &middle;
    }
    *out = *max;
}

void max_subarray_entity(int *buf, int n, struct max_subarray_entity *out) {
    if (n > 0) {
        max_subarray_entity_impl(buf, 0, n-1, out);
    }
}
