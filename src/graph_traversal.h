#ifndef GRAPH_TRAVERSAL_H
#define GRAPH_TRAVERSAL_H

#include "graph.h" // Needs Graph definition
#include <stdbool.h> // For bool array in DFS state if needed

// Callback function type to be called when a vertex is visited
typedef void (*VisitCallback)(int vertex_data);

// Breadth-First Search Traversal
// Traverses the graph starting from start_vertex.
// Calls visit_callback for each visited vertex.
void bfs_traversal(Graph* graph, int start_vertex, VisitCallback visit_callback);

// Depth-First Search Traversal
// Traverses the graph starting from start_vertex.
// Calls visit_callback for each visited vertex.
void dfs_traversal(Graph* graph, int start_vertex, VisitCallback visit_callback);

#endif // GRAPH_TRAVERSAL_H
