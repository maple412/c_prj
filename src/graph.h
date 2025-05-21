#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Node in the adjacency list
typedef struct AdjListNode {
    int dest_vertex;           // Destination vertex of an edge
    // int weight;             // Optional: Weight of the edge
    struct AdjListNode *next;  // Pointer to the next node in the list
} AdjListNode;

// Adjacency list for a single vertex
typedef struct {
    AdjListNode *head;         // Pointer to the head of the list
} AdjList;

// Graph structure
typedef struct {
    int num_vertices;          // Total number of vertices in the graph
    AdjList *adj_lists;        // Array of adjacency lists (size: num_vertices)
    bool is_directed;          // true if the graph is directed, false otherwise
} Graph;

// Graph lifecycle functions
Graph* create_graph(int num_vertices, bool is_directed);
void destroy_graph(Graph* graph);

// Graph modification functions
// For unweighted graphs. Add 'int weight' param for weighted graphs.
void add_edge(Graph* graph, int src_vertex, int dest_vertex);
// void remove_edge(Graph* graph, int src_vertex, int dest_vertex); // Optional

// Graph utility functions
void print_graph(Graph* graph); // Prints the adjacency list representation

#endif // GRAPH_H
