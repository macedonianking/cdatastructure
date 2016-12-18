#include "chapter9/chapter9_3_1.h"

#include "chapter9/chapter9.h"
#include "macros.h"
#include "util.h"



static void init_graph_data(struct graph_t *graph) {
    struct edge_pair pair_data[] = {
        {1, 2},
        {1, 4},
        {2, 5},
        {3, 1},
        {3, 6},
        {4, 3},
        {4, 6},
        {4, 7},
        {5, 4},
        {5, 7},
        {7, 6},
    };
    struct edge_pair *ptr;
    int k1, k2;
    
    init_graph(graph, 12);
    for (int i = 0; i < NARRAY(pair_data); ++i) {
        ptr = pair_data + i;
        k1 = graph_sure_get_vertex_index(graph, '0' + ptr->k1);
        k2 = graph_sure_get_vertex_index(graph, '0' + ptr->k2);
        DCHECK(k1 != -1 && k2 != -1);
        graph_add_edge(graph, k1, k2);
    }
}

static struct vertex_node_t *find_list_head_from_node(struct list_head *list, int k) {
    struct vertex_node_t *result = NULL;
    struct vertex_node_t *node;

    LIST_FOR_EACH_ENTRY(node, list, node) {
        if (node->k == k) {
            result = node;
            break;
        }
    }
    return result;
}

// 收集curr顶点下一步处理的数据
static void collect_next_list(struct graph_t *graph, int k, struct list_head *next_list) {
    struct g_edge_t *edge;
    struct vertex_t *end;
    struct vertex_node_t *node;
    struct vertex_t *curr;

    curr = graph->queue + k;
    LIST_FOR_EACH_ENTRY(edge, &curr->start_list, start_node) {
        end = graph->queue + edge->end;
        if (end->state == -1 && !find_list_head_from_node(next_list, edge->end)) {
            node = alloc_vertex_node(edge->end);
            list_add_tail(&node->node, next_list);
        }
    }
}

// dump最短路径
static void dump_shortest_path_for_graph(struct graph_t *graph, int s) {
    int d;
    DEFINE_LIST_HEAD(curr_list);
    DEFINE_LIST_HEAD(next_list);
    struct vertex_node_t *ptr;

    for (int i = 0; i < graph->size; ++i) {
        graph->queue[i].state = -1;
    }
    graph->queue[s].state = 0;

    collect_next_list(graph, s, &curr_list);
    d = 1;
    while (!list_empty(&curr_list)) {
        DCHECK(list_empty(&next_list));
        fprintf(stdout , "%d :", d);
        LIST_FOR_EACH_ENTRY(ptr, &curr_list, node) {
            fprintf(stdout, "%c ", graph->queue[ptr->k].name);
            graph->queue[ptr->k].state = d;
        }
        LIST_FOR_EACH_ENTRY(ptr, &curr_list, node) {
            collect_next_list(graph, ptr->k, &next_list);
        }
        fputc('\n', stdout);
        // 把next_list的内容移动到curr_list中
        free_vertex_node_list(&curr_list);
        list_move(&next_list, &curr_list);
        ++d;
    }
}

void chapter9_3_1_tutorial() {
    struct graph_t graph;
    int k;

    init_graph_data(&graph);
    k = graph_get_vertex_index(&graph, '3');
    DCHECK(k != -1);
    dump_shortest_path_for_graph(&graph, k);
    free_graph(&graph);
}

static void init_graph_data_figure_9_80(struct graph_t *graph) {
    init_graph(graph, 12);
    graph_add_edge_help(graph, 'A', 2, 'B', 5, 'C', 3);
    graph_add_edge_help(graph, 'B', 3, 'G', 1, 'E', 3, 'C', 2);
    graph_add_edge_help(graph, 'C', 2, 'E', 7, 'D', 7);
    graph_add_edge_help(graph, 'D', 2, 'A', 2, 'F', 6);
    graph_add_edge_help(graph, 'E', 2, 'D', 2, 'F', 1);
    graph_add_edge_help(graph, 'G', 1, 'E', 1);
}

void chapter9_5_b_problem() {
    struct graph_t graph;
    int k;

    init_graph_data_figure_9_80(&graph);
    k = graph_get_vertex_index(&graph, 'B');
    DCHECK(k != -1);
    dump_shortest_path_for_graph(&graph, k);
    free_graph(&graph);
}