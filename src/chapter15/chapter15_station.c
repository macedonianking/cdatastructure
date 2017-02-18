#include "chapter15/chapter15_station.h"

#include <stdio.h>

struct station {
    int times; // 本站点的生产时间
    int move_times; // 从其他line的前一个节点移动到本节点的时间
    int min_times; // 到达本站的最优化时间
    int min_line; // 达成最优化时间的上一个节点的最佳移动方式
};

static inline 
void station_problem(struct station *station0,
                     struct station *station1,
                     int count) {
    int time0, time1;
    station0->min_times = station0->times;
    station0->min_line = 0;
    station1->min_times = station1->times;
    station1->min_line = 1;

    for (int i = 2; i < count; ++i) {
        time0 = station0[i-1].min_times + station0[i].times;
        time1 = station1[i-1].min_times + station0[i].move_times + station0[i].times;
        if (time0 <= time1) {
            station0[i].min_times = time0;
            station0[i].min_line = 0;
        }  else {
            station0[i].min_times = time1;
            station0[i].min_line = 1;
        }

        time0 = station0[i-1].min_times + station1[i].move_times + station1[i].times;
        time1 = station1[i-1].min_times + station1[i].times;
        if (time1 <= time0) {
            station1[i].min_times = time1;
            station1[i].min_line = 1;
        } else {
            station1[i].min_times = time0;
            station1[i].min_line = 0;
        }
    }
}

static inline void print_station_internal(struct station *station0,
                                          struct station *station1,
                                          int line, int i) {
    struct station *p;

    p = line ? station1 + i : station0 + i;
    if (i != 0) {
        print_station_internal(station0, station1, p->min_line, i-1);
    }
    fprintf(stdout, "line %d, station %d\n", line, i);
}

static inline void print_station(struct station *station0, struct station *station1,
                                 int count) {
    int line = station0[count-1].min_times <= station1[count-1].min_times ? 0 : 1;
    print_station_internal(station0, station1, line, count-1);
}

void chapter15_main_station() {

}
