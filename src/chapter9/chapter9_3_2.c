#include "chapter9/chapter9_3_2.h"

#include "chapter9/chapter9.h"
#include "macros.h"

struct step_node {
    struct list_head node;  // list node
    int k;                  // curr node
    int start;              // from start
    int distance;           // target distance
};

static struct step_node *alloc_step_node(int k, int start) {
    struct step_node *ptr = (struct step_node *) malloc(sizeof(struct step_node));
    DCHECK(ptr);
    INIT_LIST_HEAD(&ptr->node);
    ptr->k = k;
    ptr->start = start;
    return ptr;
}

static void release_step_node_list(struct list_head *list) {
    struct step_node *ptr;

    LIST_FOR_EACH_ENTRY_SAFE(ptr, list, node) {
        list_del(&ptr->node);
        free(ptr);
    }
}

static void init_graph_data_figure_9_20(struct graph_t *graph) {
    init_graph(graph, 20);
    graph_add_edge_help(graph, '1', 2, '2', 2, '4', 1);
    graph_add_edge_help(graph, '2', 2, '4', 3, '5', 10);
    graph_add_edge_help(graph, '3', 2, '1', 4, '6', 5);
    graph_add_edge_help(graph, '4', 4, '3', 2, '6', 8, '7', 4, '5', 2);
    graph_add_edge_help(graph, '5', 1, '7', 6);
    graph_add_edge_help(graph, '7', 1, '6', 1);
}

static struct step_node *find_step_node_from_list(struct list_head *list, int k) {
    struct step_node *ptr;
    struct step_node *ret;

    ret = NULL;
    LIST_FOR_EACH_ENTRY(ptr, list, node) {
        if (ptr->k == k) {
            ret = ptr;
            break;
        }
    }
    return ret;
}

static void dump_graph_path(struct graph_t *graph, int s, int k) {
    if (s != k) {
        dump_graph_path(graph, s, graph->queue[k].state);
    }
    fprintf(stdout, "%c ", graph->queue[k].name);
}

static void dump_graph_shortest_path(struct graph_t *graph, int s) {
    struct vertex_t *ptr;

    fputs("Shortest path:\n", stdout);
    for (int i = 0; i < graph->size; ++i) {
        ptr = graph->queue + i;
        fprintf(stdout, "%c(%d) : ", ptr->name, ptr->distance);
        dump_graph_path(graph, s, i);
        fputc('\n', stdout);
    }
}

// 获取下一步要处理的节点
static void collect_next_list(struct graph_t *graph, int s, struct list_head *list) {
    struct vertex_t *start;
    struct vertex_t *end;
    struct g_edge_t *edge;
    struct step_node *node; 

    start = graph->queue + s;
    LIST_FOR_EACH_ENTRY(edge, &start->start_list, start_node) {
        end = graph->queue + edge->end;
        if (end->distance == -1 || end->distance > start->distance + edge->weight) {
            node = find_step_node_from_list(list, edge->end);
            if (node == NULL) {
                node = alloc_step_node(edge->end, s);
                node->distance = start->distance + edge->weight;
                list_add_tail(&node->node, list);
            } else if(node->distance > start->distance + edge->weight) {
                node->start = s;
                node->distance = start->distance + edge->weight;
            }
        }
    }
}

// 迪杰斯特拉算法
static void dijkstra(struct graph_t *graph, int s) {
    struct list_head curr_list;
    struct list_head next_list;
    struct step_node *node;
    INIT_LIST_HEAD(&curr_list);
    INIT_LIST_HEAD(&next_list);
    
    graph->queue[s].distance = 0;
    graph->queue[s].state = s;
    collect_next_list(graph, s, &curr_list);

    while (!list_empty(&curr_list)) {
        DCHECK(list_empty(&next_list));
        
        LIST_FOR_EACH_ENTRY(node, &curr_list, node) {
            graph->queue[node->k].distance = node->distance;
            graph->queue[node->k].state = node->start;
        }
        LIST_FOR_EACH_ENTRY(node, &curr_list, node) {
            collect_next_list(graph, node->k, &next_list);
        }
        release_step_node_list(&curr_list);
        DCHECK(list_empty(&curr_list));
        list_move(&next_list, &curr_list);
    }
}

void chapter9_3_2_tutorial() {
    struct graph_t graph;
    int s;
    
    init_graph_data_figure_9_20(&graph);
    s = graph_get_vertex_index(&graph, '1');
    DCHECK(s != -1);
    dijkstra(&graph, s);
    dump_graph_shortest_path(&graph, s);
    free_graph(&graph);
}

void init_graph_data_figure_9_80(struct graph_t *graph);

void chapter9_5_a_problem() {
    struct graph_t graph;
    int s;
    
    init_graph_data_figure_9_80(&graph);
    s = graph_get_vertex_index(&graph, 'A');
    DCHECK(s != -1);
    dijkstra(&graph, s);
    dump_graph_shortest_path(&graph, s);
    free_graph(&graph);
}