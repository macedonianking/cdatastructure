#include "chapter9/chapter9.h"

#include <stdlib.h>

#include "macros.h"
#include "util.h"

static inline void init_vertex(struct vertex_t *ptr) {
    ptr->indegree = 0;
    ptr->state = 0;
    ptr->name = '&';
    INIT_LIST_HEAD(&ptr->start_list);
    INIT_LIST_HEAD(&ptr->end_list);
}

void init_graph(struct graph_t *graph, int capacity) {
    graph->capacity = capacity;
    graph->size = 0;
    graph->queue = (struct vertex_t*) malloc(sizeof(struct vertex_t) * capacity);
    DCHECK(graph->queue);
    for (int i = 0; i < graph->capacity; ++i) {
        init_vertex(graph->queue + i);
    }
}

void free_graph(struct graph_t *ptr) {
    free(ptr->queue);
    ptr->queue = NULL;
    ptr->capacity = ptr->size = 0;
}

void reset_graph_indegree(struct graph_t *ptr) {
    struct vertex_t *start;
    struct g_edge_t *edge;
    for (int i = 0; i < ptr->size; ++i) {
        ptr->queue[i].indegree = 0;
    }
    
    for (int i = 0; i < ptr->size; ++i) {
        start = &ptr->queue[i];
        LIST_FOR_EACH_ENTRY(edge, &start->start_list, start_node) {
            ptr->queue[edge->end].indegree++;
        }
    }
}

static struct g_edge_t *alloc_edge_object() {
    struct g_edge_t *ptr = (struct g_edge_t*) malloc(sizeof(struct g_edge_t));
    memset(ptr, 0, sizeof(struct g_edge_t));
    INIT_LIST_HEAD(&ptr->start_node);
    INIT_LIST_HEAD(&ptr->end_node);
    return ptr;
}

// return the edge object to add other properties.
struct g_edge_t* graph_add_edge(struct graph_t *graph, int start, int end) {
    struct vertex_t *ptr;
    struct g_edge_t *edge = alloc_edge_object();
    edge->start = start;
    edge->end = end;
    
    ptr = &graph->queue[start];
    list_add_tail(&edge->start_node, &ptr->start_list);
    ptr = &graph->queue[end];
    list_add_tail(&edge->end_node, &ptr->end_list);
    ptr->indegree++;

    return edge;
}

void graph_extend_memory(struct graph_t *graph, int capacity) {
    struct vertex_t *ptr;
    if (capacity <= graph->capacity) {
        capacity = next_capacity(graph->capacity);
        DCHECK(capacity > 0);    
    }
    ptr = (struct vertex_t*) realloc(graph->queue, sizeof(struct vertex_t) * capacity);
    DCHECK(ptr);
    for (int i = graph->capacity; i < capacity; ++i) {
        init_vertex(graph->queue + i);
    }
    graph->capacity = capacity;
    graph->queue = ptr;
}

int graph_sure_get_vertex_index(struct graph_t *graph, char name) {
    int k1;
    k1 = graph_get_vertex_index(graph, name);
    if (k1 == -1) {
        if (graph->size == graph->capacity) {
            graph_extend_memory(graph, -1);
        }
        k1 = graph->size;
        graph->queue[graph->size++].name = name;
    }
    return k1;
}