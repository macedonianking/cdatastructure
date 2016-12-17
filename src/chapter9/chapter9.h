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
};

struct vertex_t {
    struct list_head start_list;    // edge start with this vertex
    struct list_head end_list;      // edge end with this vertex
    int indegree;                   // the indegree property of this vertex
    int state;
};

struct graph_t {
    struct vertex_t *queue;
    int size;
    int capacity;
};

// Initialize the graph capacity, allocate memory for the vertex node.
void init_graph(struct graph_t* graph, int capacity);
// free memory used by the graph object.
void free_graph(struct graph_t* graph);
// Reset the indegree properties for all nodes in the graph
void reset_graph_indegree(struct graph_t* graph);
// add one graph edge for the graph
struct g_edge_t *graph_add_edge(struct graph_t *graph, int start, int end);

#endif // CHAPTER9_CHAPTER9_H
