#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "graph.h" // For Graph structure

// Dijkstra's Algorithm for non-negative weights
// Calculates shortest paths from start_vertex to all other vertices.
// graph: The input graph (must be weighted, non-negative weights).
// start_vertex: The source vertex.
// distances: Output array (size num_vertices) to store shortest distances.
//            Caller must ensure this array is allocated.
// predecessors: Output array (size num_vertices) to store predecessor in shortest path.
//               Caller must ensure this array is allocated.
// Returns 0 on success, -1 on error (e.g., graph is NULL, start_vertex invalid).
int dijkstra(Graph* graph, int start_vertex, int* distances, int* predecessors);

// Bellman-Ford Algorithm for graphs that may include negative weight edges
// Detects negative weight cycles reachable from start_vertex.
// graph: The input graph (must be weighted).
// start_vertex: The source vertex.
// distances: Output array (size num_vertices) to store shortest distances.
//            Caller must ensure this array is allocated.
// predecessors: Output array (size num_vertices) to store predecessor in shortest path.
//               Caller must ensure this array is allocated.
// Returns 0 on success, 1 if a negative weight cycle is detected, -1 on error.
int bellman_ford(Graph* graph, int start_vertex, int* distances, int* predecessors);

#endif // SHORTEST_PATH_H
