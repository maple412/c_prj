#include "graph_traversal.h"
#include "graph.h" 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// --- Static Helper Function Prototypes ---

// For DFS
static void dfs_recursive(Graph* graph, int current_vertex, bool* visited, VisitCallback visit_callback);

// For BFS (Simple Array-based Queue)
typedef struct {
    int* items;
    int front;
    int rear;
    int capacity;
    // int count; // Not strictly needed if using front/rear carefully
} SimpleQueue;

static SimpleQueue* create_simple_queue(int capacity);
static void free_simple_queue(SimpleQueue* queue);
static bool is_simple_queue_empty(SimpleQueue* queue);
static bool is_simple_queue_full(SimpleQueue* queue); // Optional, good for fixed capacity
static void enqueue_simple(SimpleQueue* queue, int item);
static int dequeue_simple(SimpleQueue* queue);


// --- Array-based Queue Static Helper Implementations for BFS ---

static SimpleQueue* create_simple_queue(int capacity) {
    if (capacity <= 0) return NULL;
    SimpleQueue* queue = (SimpleQueue*)malloc(sizeof(SimpleQueue));
    if (queue == NULL) {
        perror("Failed to allocate SimpleQueue");
        return NULL;
    }
    queue->items = (int*)malloc(sizeof(int) * capacity);
    if (queue->items == NULL) {
        perror("Failed to allocate items array for SimpleQueue");
        free(queue);
        return NULL;
    }
    queue->capacity = capacity;
    queue->front = -1; // Or 0, depending on empty/full logic
    queue->rear = -1;  // Or 0
    return queue;
}

static void free_simple_queue(SimpleQueue* queue) {
    if (queue == NULL) {
        return;
    }
    free(queue->items);
    free(queue);
}

static bool is_simple_queue_empty(SimpleQueue* queue) {
    if (queue == NULL) return true; // Or handle error
    return queue->front == -1 || queue->front > queue->rear;
}

static bool is_simple_queue_full(SimpleQueue* queue) {
    if (queue == NULL) return true; // Or handle error
    return (queue->rear + 1) % queue->capacity == queue->front; // For circular queue
    // For simple linear queue (as likely intended by front/rear = -1 start):
    // return queue->rear == queue->capacity - 1;
}


static void enqueue_simple(SimpleQueue* queue, int item) {
    if (queue == NULL || queue->rear == queue->capacity - 1) { // Simple linear queue full check
        // fprintf(stderr, "Queue is full. Cannot enqueue.\n"); // Production code might realloc
        return;
    }
    if (queue->front == -1) { // First element
        queue->front = 0;
    }
    queue->rear++;
    queue->items[queue->rear] = item;
}

static int dequeue_simple(SimpleQueue* queue) {
    if (is_simple_queue_empty(queue)) {
        // fprintf(stderr, "Queue is empty. Cannot dequeue.\n");
        return -1; // Error or sentinel
    }
    int item = queue->items[queue->front];
    if (queue->front == queue->rear) { // Last element
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front++;
    }
    return item;
}


// --- Public Traversal Function Implementations ---

void bfs_traversal(Graph* graph, int start_vertex, VisitCallback visit_callback) {
    if (graph == NULL) {
        fprintf(stderr, "Error: Graph is NULL for BFS.\n");
        return;
    }
    if (graph->num_vertices == 0) {
        return; // Nothing to traverse
    }
    if (start_vertex < 0 || start_vertex >= graph->num_vertices) {
        fprintf(stderr, "Error: Invalid start_vertex %d for BFS (num_vertices: %d).\n", start_vertex, graph->num_vertices);
        return;
    }

    bool* visited = (bool*)calloc(graph->num_vertices, sizeof(bool));
    if (visited == NULL) {
        perror("Failed to allocate memory for visited array in BFS");
        return;
    }

    SimpleQueue* q = create_simple_queue(graph->num_vertices);
    if (q == NULL) {
        free(visited);
        return;
    }

    visited[start_vertex] = true;
    enqueue_simple(q, start_vertex);

    while (!is_simple_queue_empty(q)) {
        int current_vertex = dequeue_simple(q);
        if (visit_callback != NULL) {
            visit_callback(current_vertex);
        }

        AdjListNode* temp = graph->adj_lists[current_vertex].head;
        while (temp != NULL) {
            int adj_v = temp->dest_vertex;
            if (!visited[adj_v]) {
                visited[adj_v] = true;
                enqueue_simple(q, adj_v);
            }
            temp = temp->next;
        }
    }

    free(visited);
    free_simple_queue(q);
}

static void dfs_recursive(Graph* graph, int current_vertex, bool* visited, VisitCallback visit_callback) {
    visited[current_vertex] = true;
    if (visit_callback != NULL) {
        visit_callback(current_vertex);
    }

    AdjListNode* temp = graph->adj_lists[current_vertex].head;
    while (temp != NULL) {
        int adjacent_vertex = temp->dest_vertex;
        if (!visited[adjacent_vertex]) {
            dfs_recursive(graph, adjacent_vertex, visited, visit_callback);
        }
        temp = temp->next;
    }
}

void dfs_traversal(Graph* graph, int start_vertex, VisitCallback visit_callback) {
    if (graph == NULL) {
        fprintf(stderr, "Error: Graph is NULL for DFS.\n");
        return;
    }
    if (graph->num_vertices == 0) {
        return; // Nothing to traverse
    }
     if (start_vertex < 0 || start_vertex >= graph->num_vertices) {
        fprintf(stderr, "Error: Invalid start_vertex %d for DFS (num_vertices: %d).\n", start_vertex, graph->num_vertices);
        return;
    }
    
    bool* visited = (bool*)calloc(graph->num_vertices, sizeof(bool));
    if (visited == NULL) {
        perror("Failed to allocate memory for visited array in DFS");
        return;
    }

    dfs_recursive(graph, start_vertex, visited, visit_callback);
    
    // If you want to ensure all components of a potentially disconnected graph are visited:
    // for (int i = 0; i < graph->num_vertices; ++i) {
    //     if (!visited[i]) {
    //         dfs_recursive(graph, i, visited, visit_callback);
    //     }
    // }

    free(visited);
}
