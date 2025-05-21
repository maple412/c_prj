#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// --- Graph Lifecycle Functions ---

Graph* create_graph(int num_vertices, bool is_directed) {
    if (num_vertices <= 0) {
        fprintf(stderr, "Error: Number of vertices must be positive.\n");
        return NULL;
    }

    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (graph == NULL) {
        perror("Failed to allocate memory for Graph");
        return NULL;
    }

    graph->num_vertices = num_vertices;
    graph->is_directed = is_directed;

    // Allocate memory for an array of adjacency lists
    graph->adj_lists = (AdjList*)malloc(num_vertices * sizeof(AdjList));
    if (graph->adj_lists == NULL) {
        perror("Failed to allocate memory for adjacency lists");
        free(graph);
        return NULL;
    }

    // Initialize each adjacency list head to NULL
    for (int i = 0; i < num_vertices; i++) {
        graph->adj_lists[i].head = NULL;
    }

    return graph;
}

void destroy_graph(Graph* graph) {
    if (graph == NULL) {
        return;
    }

    // Free each adjacency list
    for (int i = 0; i < graph->num_vertices; i++) {
        AdjListNode* current_node = graph->adj_lists[i].head;
        AdjListNode* next_node;
        while (current_node != NULL) {
            next_node = current_node->next;
            free(current_node);
            current_node = next_node;
        }
    }

    // Free the array of adjacency lists
    free(graph->adj_lists);
    // Free the graph structure itself
    free(graph);
}

// --- Graph Modification Functions ---

// Creates a new adjacency list node
static AdjListNode* create_adj_list_node(int dest_vertex) {
    AdjListNode* new_node = (AdjListNode*)malloc(sizeof(AdjListNode));
    if (new_node == NULL) {
        perror("Failed to allocate memory for AdjListNode");
        return NULL; 
    }
    new_node->dest_vertex = dest_vertex;
    new_node->next = NULL;
    return new_node;
}

void add_edge(Graph* graph, int src_vertex, int dest_vertex) {
    if (graph == NULL) {
        fprintf(stderr, "Error: Graph is NULL.\n");
        return;
    }
    if (src_vertex < 0 || src_vertex >= graph->num_vertices ||
        dest_vertex < 0 || dest_vertex >= graph->num_vertices) {
        fprintf(stderr, "Error: Invalid vertex number for add_edge (%d -> %d, num_vertices: %d).\n", 
                src_vertex, dest_vertex, graph->num_vertices);
        return;
    }

    // Add an edge from src_vertex to dest_vertex
    AdjListNode* new_node_dest = create_adj_list_node(dest_vertex);
    if (new_node_dest == NULL) return; // Error handled in create_adj_list_node

    new_node_dest->next = graph->adj_lists[src_vertex].head;
    graph->adj_lists[src_vertex].head = new_node_dest;

    // If the graph is undirected, add an edge from dest_vertex to src_vertex as well
    if (!graph->is_directed) {
        AdjListNode* new_node_src = create_adj_list_node(src_vertex);
        if (new_node_src == NULL) {
            // If this allocation fails, the graph is in an inconsistent state for undirected.
            // For simplicity here, we don't roll back the first edge.
            // A more robust implementation might handle this.
            return; 
        }
        new_node_src->next = graph->adj_lists[dest_vertex].head;
        graph->adj_lists[dest_vertex].head = new_node_src;
    }
}

// --- Graph Utility Functions ---

void print_graph(Graph* graph) {
    if (graph == NULL) {
        printf("Graph is NULL.\n");
        return;
    }
    printf("Graph Representation (Adjacency List, %d Vertices, %s):\n",
           graph->num_vertices, graph->is_directed ? "Directed" : "Undirected");

    for (int i = 0; i < graph->num_vertices; i++) {
        AdjListNode* current_node = graph->adj_lists[i].head;
        printf("Vertex %d: ", i);
        if (current_node == NULL) {
            printf("-> NULL");
        }
        while (current_node != NULL) {
            printf("-> %d", current_node->dest_vertex);
            // if (current_node->weight != 0) printf("(w:%d)", current_node->weight); // If weighted
            current_node = current_node->next;
        }
        printf("\n");
    }
}
