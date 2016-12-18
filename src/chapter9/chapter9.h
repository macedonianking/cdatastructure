#ifndef CHAPTER9_CHAPTER9_H
#define CHAPTER9_CHAPTER9_H

#include "list.h"

struct edge_pair {
    int k1, k2;
};

struct g_edge_t {
    struct list_head start_node;
    struct list_head end_node;
    int start;
    int end;
    int weight;
};

struct vertex_t {
    struct list_head start_list;    // edge start with this vertex
    struct list_head end_list;      // edge end with this vertex
    int indegree;                   // the indegree property of this vertex
    int state;
    int distance;
    char name;
};

struct graph_t {
    struct vertex_t *queue;
    int size;
    int capacity;
};

struct vertex_node_t {
    struct list_head node;
    int k;
};



// Initialize the graph capacity, allocate memory for the vertex node.
void init_graph(struct graph_t* graph, int capacity);
// free memory used by the graph object.
void free_graph(struct graph_t* graph);
// Reset the indegree properties for all nodes in the graph
void reset_graph_indegree(struct graph_t* graph);
// add one graph edge for the graph
struct g_edge_t *graph_add_edge(struct graph_t *graph, int start, int end);
// extend graph memory
void graph_extend_memory(struct graph_t *graph, int capacity);

// set vertex name char
static inline void graph_set_vertex_name(struct graph_t *graph, int i, char name) {
    graph->queue[i].name = name;
}

// get vertex index from name char.
static inline int graph_get_vertex_index(struct graph_t *graph, char name) {
    for (int i = 0; i < graph->size; ++i) {
        if (graph->queue[i].name == name) {
            return i;
        }
    }
    return -1;
}

struct vertex_node_t *alloc_vertex_node(int k);
void free_vertex_node_list(struct list_head *list);

int graph_sure_get_vertex_index(struct graph_t *graph, char name);
void graph_add_edge_help(struct graph_t *graph, char start, int n, ...);

#endif // CHAPTER9_CHAPTER9_H
