#include "chapter9/topological_sort.h"

#include "chapter9/chapter9.h"
#include "macros.h"

// 图形的数据
static struct edge_pair graph_data[] = {
    {1, 2},
    {1, 4},
    {2, 4},
    {2, 5},
    {3, 6},
    {4, 3},
    {4, 5},
    {4, 5},
    {4, 7},
    {5, 7},
    {7, 6},
};

struct edge_pair_weight {
    char start;
    char end;
    int weight;
};

struct vertex_node_t {
    struct list_head node;
    int k1;
};

// 在删除k1节点后，减少对应的变得边的结束节点的indgree的值, 如果indegree的值位0，那么放入zero_queue中
static void on_remove_vertex(struct graph_t *graph, int k1, struct list_head *zero_queue) {
    struct vertex_t *vertex, *end_vertex;
    struct vertex_node_t *vertex_node;
    struct g_edge_t *ptr;

    vertex = graph->queue + k1;
    LIST_FOR_EACH_ENTRY(ptr, &vertex->start_list, start_node) {
        end_vertex = graph->queue + ptr->end;
        if (!--end_vertex->indegree) {
            vertex_node = (struct vertex_node_t*) malloc(sizeof(struct vertex_node_t));
            vertex_node->k1 = ptr->end;
            INIT_LIST_HEAD(&vertex_node->node);
            list_add_tail(&vertex_node->node, zero_queue);
        }
    }
}

// 显示拓扑顺序
static void dump_topological_order(struct graph_t *graph) {
    struct vertex_t *curr;
    struct vertex_node_t *node;
    struct list_head zero_queue, result_queue;
    INIT_LIST_HEAD(&zero_queue);
    INIT_LIST_HEAD(&result_queue);
    int n;

    // 构建列表
    for (int i = 0; i < graph->size; ++i) {
        curr = graph->queue + i;
        curr->state = 1;
        if (!curr->indegree) {
            node = (struct vertex_node_t*) malloc(sizeof(struct vertex_node_t));
            node->k1 = i;
            INIT_LIST_HEAD(&node->node);
            list_add_tail(&node->node, &zero_queue);
        }
    }

    // 得到拓扑顺序
    n = graph->size;
    while (!list_empty(&zero_queue)) {
        node = list_entry(zero_queue.next, struct vertex_node_t, node);
        list_del(&node->node);
        list_add_tail(&node->node, &result_queue);
        on_remove_vertex(graph, node->k1, &zero_queue);
        --n;
        graph->queue[node->k1].state= 0;
    }

    // 是循序结构，不能输出拓扑顺序
    if (n != 0) {
        fprintf(stdout, "the graph:%p has cycle\n", graph);
        goto out;
        return;
    }

    // 输出顺序，释放内存
    fprintf(stdout, "Order: ");
    while (!list_empty(&result_queue)) {
        node = list_entry(result_queue.next, struct vertex_node_t, node);
        list_del(&node->node);
        fprintf(stdout, "%c ", graph->queue[node->k1].name);
        free(node);
    }
    fputc('\n', stdout);

out:
    // 释放内存
    while(!list_empty(&result_queue)) {
        node = list_entry(result_queue.next, struct vertex_node_t, node);
        list_del(&node->node);
    }

    // 重置indegree的属性
    reset_graph_indegree(graph);
    return;
}

void chapter9_2_tutorial() {
    struct graph_t graph;
    struct edge_pair *pair;
    int k1, k2;

    init_graph(&graph, 10);
    for (int i = 0; i < NARRAY(graph_data); ++i) {
        pair = graph_data + i;
        k1 = graph_sure_get_vertex_index(&graph, '0' + pair->k1);
        k2 = graph_sure_get_vertex_index(&graph, '0' + pair->k2);
        DCHECK(k1 != -1 && k2 != -1);
        graph_add_edge(&graph, k1, k2);
    }

    // 输出拓扑顺序
    dump_topological_order(&graph);
    free_graph(&graph);
}

// 输入数据
// 9.1，
static struct edge_pair_weight graph_edge_weight[] = {
    {'A', 'B', 2},
    {'A', 'E', 2},
    {'B', 'C', 2},
    {'C', 't', 4},
    {'D', 'A', 3},
    {'D', 'E', 3},
    {'E', 'C', 2},
    {'E', 'F', 3},
    {'E', 'I', 3},
    {'F', 'C', 1},
    {'F', 't', 3},
    {'G', 'D', 2},
    {'G', 'E', 1},
    {'G', 'H', 6},
    {'H', 'E', 2},
    {'H', 'I', 6},
    {'I', 'F', 1},
    {'I', 't', 4},
    {'s', 'A', 1},
    {'s', 'D', 4},
    {'s', 'G', 6},
};

static void init_graph_vertex_data(struct graph_t *graph) {
    char name[] = "ABCDEFGHIst";
    int length = NARRAY(name) - 1;
    int k1;

    for (int i = 0; i < length; ++i) {
        k1 = graph_get_vertex_index(graph, name[i]);
        if (k1 == -1) {
            if (graph->size == graph->capacity) {
                graph_extend_memory(graph, -1);
            }
            graph->queue[graph->size].name = name[i];
            ++graph->size;
        }
    }
}

// 初始化图形数据
static void init_graph_edge_weight_data(struct graph_t *graph) {
    struct edge_pair_weight *ptr;
    struct g_edge_t *edge;
    int k1, k2;

   
    init_graph_vertex_data(graph);
    for (int i = 0; i < NARRAY(graph_edge_weight); ++i) {
        ptr = graph_edge_weight + i;
        k1 = graph_get_vertex_index(graph, ptr->start);
        k2 = graph_get_vertex_index(graph, ptr->end);
        DCHECK(k1 != -1 && k2 != -1);
        edge = graph_add_edge(graph, k1, k2);
        edge->weight = ptr->weight;
    }
}

void chapter9_1_problem() {
    struct graph_t graph;

    init_graph(&graph, 12);
    init_graph_edge_weight_data(&graph);
    dump_topological_order(&graph);
    free_graph(&graph);
}