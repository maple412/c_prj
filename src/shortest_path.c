#include "shortest_path.h"
#include "graph.h" // For Graph, AdjListNode
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h> // For INT_MAX

// --- Static Helper Function Prototypes for Dijkstra's Min-Priority Queue ---

typedef struct MinPriorityQueueNode {
    int vertex;
    int distance; 
} MinPriorityQueueNode;

typedef struct MinPriorityQueue {
    MinPriorityQueueNode* nodes; // Array of nodes (vertex, distance)
    int* pos;                // pos[v] stores the heap array index of vertex v
    int size;
    int capacity;
} MinPriorityQueue;

static MinPriorityQueue* pq_create(int capacity);
static void pq_destroy(MinPriorityQueue* pq);
static bool pq_is_empty(MinPriorityQueue* pq);
static void pq_swap_nodes(MinPriorityQueue* pq, int i, int j);
static void pq_heapify_down(MinPriorityQueue* pq, int idx);
static void pq_heapify_up(MinPriorityQueue* pq, int idx);
static void pq_insert_or_decrease_key(MinPriorityQueue* pq, int vertex, int distance);
static MinPriorityQueueNode pq_extract_min(MinPriorityQueue* pq);


// --- Min-Priority Queue Static Helper Implementations ---

static MinPriorityQueue* pq_create(int capacity) {
    if (capacity <= 0) return NULL;
    MinPriorityQueue* pq = (MinPriorityQueue*)malloc(sizeof(MinPriorityQueue));
    if (pq == NULL) {
        perror("Failed to allocate MinPriorityQueue");
        return NULL;
    }
    // nodes array stores the actual MinPriorityQueueNode structs
    pq->nodes = (MinPriorityQueueNode*)malloc(sizeof(MinPriorityQueueNode) * capacity);
    if (pq->nodes == NULL) {
        perror("Failed to allocate nodes array for MinPriorityQueue");
        free(pq);
        return NULL;
    }
    pq->pos = (int*)malloc(sizeof(int) * capacity);
    if (pq->pos == NULL) {
        perror("Failed to allocate pos array for MinPriorityQueue");
        free(pq->nodes);
        free(pq);
        return NULL;
    }
    pq->capacity = capacity;
    pq->size = 0;
    for (int i = 0; i < capacity; ++i) {
        pq->pos[i] = -1; // Vertex i is not in PQ initially
    }
    return pq;
}

static void pq_destroy(MinPriorityQueue* pq) {
    if (pq == NULL) return;
    free(pq->nodes);
    free(pq->pos);
    free(pq);
}

static bool pq_is_empty(MinPriorityQueue* pq) {
    if (pq == NULL) return true;
    return pq->size == 0;
}

// Swaps nodes at heap indices i and j
static void pq_swap_nodes(MinPriorityQueue* pq, int i, int j) {
    MinPriorityQueueNode temp_node_data = pq->nodes[i];
    int vertex_i = pq->nodes[i].vertex;
    int vertex_j = pq->nodes[j].vertex;

    // Swap nodes
    pq->nodes[i] = pq->nodes[j];
    pq->nodes[j] = temp_node_data;

    // Update positions
    pq->pos[vertex_i] = j;
    pq->pos[vertex_j] = i;
}

static void pq_heapify_down(MinPriorityQueue* pq, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < pq->size && pq->nodes[left].distance < pq->nodes[smallest].distance) {
        smallest = left;
    }
    if (right < pq->size && pq->nodes[right].distance < pq->nodes[smallest].distance) {
        smallest = right;
    }

    if (smallest != idx) {
        pq_swap_nodes(pq, idx, smallest);
        pq_heapify_down(pq, smallest);
    }
}

static void pq_heapify_up(MinPriorityQueue* pq, int idx) {
    int parent = (idx - 1) / 2;
    while (idx > 0 && pq->nodes[idx].distance < pq->nodes[parent].distance) {
        pq_swap_nodes(pq, idx, parent);
        idx = parent;
        parent = (idx - 1) / 2;
    }
}

static void pq_insert_or_decrease_key(MinPriorityQueue* pq, int vertex, int distance) {
    if (pq == NULL || vertex < 0 || vertex >= pq->capacity) return;

    if (pq->pos[vertex] != -1 && pq->pos[vertex] < pq->size) { // Vertex is in PQ
        // If new distance is smaller, update and heapify up
        if (distance < pq->nodes[pq->pos[vertex]].distance) {
            pq->nodes[pq->pos[vertex]].distance = distance;
            pq_heapify_up(pq, pq->pos[vertex]);
        }
    } else { // Vertex not in PQ (or was extracted)
        if (pq->size == pq->capacity) {
            // fprintf(stderr, "Priority Queue is full. Cannot insert/decrease key for vertex %d.\n", vertex);
            return; // Or handle error, realloc etc.
        }
        // Insert new node
        int new_idx = pq->size;
        pq->nodes[new_idx].vertex = vertex;
        pq->nodes[new_idx].distance = distance;
        pq->pos[vertex] = new_idx;
        pq->size++;
        pq_heapify_up(pq, new_idx);
    }
}

static MinPriorityQueueNode pq_extract_min(MinPriorityQueue* pq) {
    MinPriorityQueueNode min_node;
    if (pq_is_empty(pq)) {
        // fprintf(stderr, "Priority Queue is empty. Cannot extract min.\n");
        min_node.vertex = -1; // Indicate error/empty
        min_node.distance = INT_MAX;
        return min_node;
    }

    min_node = pq->nodes[0];
    pq->pos[min_node.vertex] = -1; // Mark as extracted

    if (pq->size > 1) {
        pq->nodes[0] = pq->nodes[pq->size - 1];
        pq->pos[pq->nodes[0].vertex] = 0; // Update pos of moved node
    }
    pq->size--;

    if (pq->size > 0) {
        pq_heapify_down(pq, 0);
    }
    
    return min_node;
}


// --- Dijkstra's Algorithm ---

int dijkstra(Graph* graph, int start_vertex, int* distances, int* predecessors) {
    if (graph == NULL) {
        fprintf(stderr, "Error: Graph is NULL for Dijkstra.\n");
        return -1;
    }
    if (graph->num_vertices == 0) { // Handle empty graph specifically
        return 0; 
    }
    if (start_vertex < 0 || start_vertex >= graph->num_vertices) {
        fprintf(stderr, "Error: Invalid start_vertex %d for Dijkstra (num_vertices: %d).\n", start_vertex, graph->num_vertices);
        return -1;
    }
    if (distances == NULL || predecessors == NULL) {
        fprintf(stderr, "Error: Output arrays (distances/predecessors) are NULL for Dijkstra.\n");
        return -1;
    }

    for (int i = 0; i < graph->num_vertices; ++i) {
        distances[i] = INT_MAX;
        predecessors[i] = -1;
    }
    distances[start_vertex] = 0;

    MinPriorityQueue* pq = pq_create(graph->num_vertices);
    if (pq == NULL) return -1; // Allocation failure

    pq_insert_or_decrease_key(pq, start_vertex, 0);
    // Optimization: could add all other vertices with INT_MAX if PQ supports it efficiently,
    // or just add them as they are reached. Current pq_insert_or_decrease_key handles both.

    while (!pq_is_empty(pq)) {
        MinPriorityQueueNode u_node = pq_extract_min(pq);
        int u = u_node.vertex;

        // If u_node.distance is INT_MAX, all remaining vertices are inaccessible from start_vertex
        if (u_node.distance == INT_MAX) {
            break; 
        }

        AdjListNode* temp = graph->adj_lists[u].head;
        while (temp != NULL) {
            int v = temp->dest_vertex;
            int weight = temp->weight;

            if (weight < 0) {
                fprintf(stderr, "Error: Dijkstra's algorithm does not support negative edge weights (edge %d->%d, weight %d).\n", u,v,weight);
                pq_destroy(pq);
                return -1; // Indicate error
            }

            if (distances[u] != INT_MAX && distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                predecessors[v] = u;
                pq_insert_or_decrease_key(pq, v, distances[v]);
            }
            temp = temp->next;
        }
    }

    pq_destroy(pq);
    return 0; // Success
}


// --- Bellman-Ford Algorithm ---

int bellman_ford(Graph* graph, int start_vertex, int* distances, int* predecessors) {
    if (graph == NULL) {
        fprintf(stderr, "Error: Graph is NULL for Bellman-Ford.\n");
        return -1;
    }
    if (graph->num_vertices == 0) { // Handle empty graph
        return 0; 
    }
    if (start_vertex < 0 || start_vertex >= graph->num_vertices) {
        fprintf(stderr, "Error: Invalid start_vertex %d for Bellman-Ford (num_vertices: %d).\n", start_vertex, graph->num_vertices);
        return -1;
    }
    if (distances == NULL || predecessors == NULL) {
        fprintf(stderr, "Error: Output arrays (distances/predecessors) are NULL for Bellman-Ford.\n");
        return -1;
    }
    
    for (int i = 0; i < graph->num_vertices; ++i) {
        distances[i] = INT_MAX;
        predecessors[i] = -1;
    }
    distances[start_vertex] = 0;

    // Relax edges V-1 times
    for (int i = 0; i < graph->num_vertices - 1; ++i) {
        for (int u = 0; u < graph->num_vertices; ++u) { // Iterate through all vertices as potential sources
            if (distances[u] == INT_MAX) continue; // Optimization: skip if u is unreachable

            AdjListNode* temp = graph->adj_lists[u].head;
            while (temp != NULL) {
                int v = temp->dest_vertex;
                int weight = temp->weight;
                if (distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                    predecessors[v] = u;
                }
                temp = temp->next;
            }
        }
    }

    // Check for negative weight cycles (V-th iteration)
    for (int u = 0; u < graph->num_vertices; ++u) {
        if (distances[u] == INT_MAX) continue;

        AdjListNode* temp = graph->adj_lists[u].head;
        while (temp != NULL) {
            int v = temp->dest_vertex;
            int weight = temp->weight;
            if (distances[u] + weight < distances[v]) {
                // To be more precise, one could mark distances[v] = -INT_MAX or similar
                // to indicate they are part of/reachable from a negative cycle.
                // For now, just returning 1 is sufficient as per spec.
                return 1; // Negative weight cycle detected
            }
            temp = temp->next;
        }
    }

    return 0; // Success, no negative cycle detected affecting paths from start
}
