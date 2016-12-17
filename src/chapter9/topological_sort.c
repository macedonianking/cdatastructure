#include "chapter9/topological_sort.h"

#include "chapter9/chapter9.h"

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
    for (int i = 1; i < graph->size; ++i) {
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
    n = graph->size - 1;
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
        fprintf(stdout, "%d ", node->k1);
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

    init_graph(&graph, 10);
    graph.size = 8;
    for (int i = 0; i < NARRAY(graph_data); ++i) {
        graph_add_edge(&graph, graph_data[i].k1, graph_data[i].k2);
    }

    // 输出拓扑顺序
    dump_topological_order(&graph);
    free_graph(&graph);
}