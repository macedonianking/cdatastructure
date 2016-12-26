#include "chapter4/algorithm4.h"

#include "util.h"
#include "macros.h"

static void find_maximum_subarray_recursive(int *buf, int s, int e, 
                                            int *outs, int *oute, int *outv) {
    int m;
    int cur_s, cur_e, cur_v;
    int out_s, out_e, out_v;
    int tmp_v;
    if (s == e) {
        *outs = s;
        *oute = e;
        *outv = buf[s];
        return;
    }

    m = (s+e) / 2;
    cur_s = m;
    cur_e = m;
    cur_v = buf[m];
    tmp_v = cur_v;
    for (int i = m-1; i >= s; --i) {
        tmp_v += buf[i];
        if (tmp_v > cur_v) {
            cur_v = tmp_v;
            cur_s = i;
        }
    }
    tmp_v = cur_v;
    for (int i = m+1; i<=e; ++i) {
        tmp_v += buf[i];
        if (tmp_v > cur_v) {
            cur_v = tmp_v;
            cur_e = i;
        }
    }

    if (s <= m-1) {
        find_maximum_subarray_recursive(buf, s, m-1, &out_s, &out_e, &out_v);
        if (out_v > cur_v) {
            cur_s = out_s;
            cur_e = out_e;
            cur_v = out_v;
        }
    }
    if (m+1 <= e) {
        find_maximum_subarray_recursive(buf, m+1, e, &out_s, &out_e, &out_v);
        if (out_v > cur_v) {
            cur_s = out_s;
            cur_e = out_e;
            cur_v = out_v;
        }
    }
    *outs = cur_s;
    *oute = cur_e;
    *outv = cur_v;
}

static void find_maximum_subarray(int *buf, int n, int *outs, int *oute, int *outv) {
    int out_s, out_e, out_v;
    find_maximum_subarray_recursive(buf, 0, n-1, &out_s, &out_e, &out_v);
    *outs = out_s;
    *oute = out_e;
    *outv = out_v;
}

void algorithm_4_1_tutorial() {
    int outs, oute, outv;
    int buf[] = {13, -3, -25, 20, -3, -16, -23, 18, 20, -7, 12, -5, -22, 15, -4, 7};
    find_maximum_subarray(buf, NARRAY(buf), &outs, &oute, &outv);
    fprintf(stdout, "outs=%d, oute=%d, outv=%d\n", outs, oute, outv);
}