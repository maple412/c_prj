#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h> // For INT_MAX

#include "../src/graph.h"           
#include "../src/shortest_path.h"   

// Helper to initialize distances and predecessors arrays
void initialize_paths(int num_vertices, int distances[], int predecessors[]) {
    if (num_vertices == 0) return; // Avoid issues with 0-size arrays if not dynamically allocated based on num_vertices
    for (int i = 0; i < num_vertices; i++) {
        distances[i] = INT_MAX;
        predecessors[i] = -1; 
    }
}

// Helper to print paths (for debugging)
void print_paths(const char* algorithm_name, int start_vertex, int num_vertices, int distances[], int predecessors[]) {
    if (num_vertices == 0) {
        printf("%s from vertex %d on empty graph: No paths.\n", algorithm_name, start_vertex);
        return;
    }
    printf("%s paths from vertex %d:\n", algorithm_name, start_vertex);
    for (int i = 0; i < num_vertices; i++) {
        if (distances[i] == INT_MAX) {
            printf("  Vertex %d: Distance = INF, Predecessor = %d\n", i, predecessors[i]);
        } else {
            printf("  Vertex %d: Distance = %d, Predecessor = %d\n", i, distances[i], predecessors[i]);
        }
    }
}

// Test function prototypes
void test_dijkstra_simple(void);
void test_dijkstra_more_complex(void);
void test_dijkstra_negative_edge(void);
void test_dijkstra_disconnected(void);
void test_dijkstra_single_node(void);
void test_dijkstra_cyclic_graph_non_negative(void);
void test_dijkstra_all_vertices_reachable(void);
void test_dijkstra_empty_graph_and_out_of_bounds(void);


void test_bellman_ford_simple(void);
void test_bellman_ford_negative_weights_no_cycle(void);
void test_bellman_ford_another_negative_no_cycle(void);
void test_bellman_ford_detects_negative_cycle_simple(void);
void test_bellman_ford_detects_negative_cycle_reachable(void);
void test_bellman_ford_detects_negative_cycle_unreachable(void);
void test_bellman_ford_no_path_to_negative_cycle(void);
void test_bellman_ford_disconnected(void);
void test_bellman_ford_single_node(void);
void test_bellman_ford_empty_graph_and_out_of_bounds(void);


// Main function to run all tests
int main() {
    printf("Running Shortest Path Algorithm tests...\n\n");

    printf("--- Dijkstra Tests ---\n");
    test_dijkstra_simple();
    test_dijkstra_more_complex();
    test_dijkstra_negative_edge();
    test_dijkstra_disconnected();
    test_dijkstra_single_node();
    test_dijkstra_cyclic_graph_non_negative();
    test_dijkstra_all_vertices_reachable();
    test_dijkstra_empty_graph_and_out_of_bounds();

    printf("\n--- Bellman-Ford Tests ---\n");
    test_bellman_ford_simple();
    test_bellman_ford_negative_weights_no_cycle();
    test_bellman_ford_another_negative_no_cycle();
    test_bellman_ford_detects_negative_cycle_simple();
    test_bellman_ford_detects_negative_cycle_reachable();
    test_bellman_ford_detects_negative_cycle_unreachable();
    test_bellman_ford_no_path_to_negative_cycle();
    test_bellman_ford_disconnected();
    test_bellman_ford_single_node();
    test_bellman_ford_empty_graph_and_out_of_bounds();

    printf("\nShortest Path Algorithm tests completed!\n");
    return 0;
}

// --- Dijkstra's Algorithm Tests ---

void test_dijkstra_simple(void) {
    printf("  Testing Dijkstra: Simple graph...\n");
    Graph* g = create_graph(5, false); 
    add_edge(g, 0, 1, 10); add_edge(g, 0, 2, 3);
    add_edge(g, 1, 2, 1);  add_edge(g, 1, 3, 2);
    add_edge(g, 2, 1, 4);  add_edge(g, 2, 3, 8); add_edge(g, 2, 4, 2);
    add_edge(g, 3, 4, 5);

    int distances[5];
    int predecessors[5];
    initialize_paths(5, distances, predecessors);

    assert(dijkstra(g, 0, distances, predecessors) == 0);
    // print_paths("Dijkstra Simple", 0, 5, distances, predecessors);
    assert(distances[0] == 0);
    assert(distances[1] == 4 && predecessors[1] == 2); // 0->2->1 (3+1=4)
    assert(distances[2] == 3 && predecessors[2] == 0);
    assert(distances[3] == 6 && predecessors[3] == 1); // 0->2->1->3 (3+1+2=6)
    assert(distances[4] == 5 && predecessors[4] == 2); // 0->2->4 (3+2=5)

    destroy_graph(g);
    printf("  test_dijkstra_simple: PASSED\n");
}

void test_dijkstra_more_complex(void) {
    printf("  Testing Dijkstra: More complex graph...\n");
    Graph* g = create_graph(6, false);
    add_edge(g, 0, 1, 7); add_edge(g, 0, 2, 9); add_edge(g, 0, 5, 14);
    add_edge(g, 1, 2, 10);add_edge(g, 1, 3, 15);
    add_edge(g, 2, 3, 11);add_edge(g, 2, 5, 2);
    add_edge(g, 3, 4, 6);
    add_edge(g, 4, 5, 9);

    int distances[6];
    int predecessors[6];
    initialize_paths(6, distances, predecessors);
    assert(dijkstra(g, 0, distances, predecessors) == 0);
    // Expected: 0->(0)
    // 0->1 (7)
    // 0->2 (9)
    // 0->2->5 (9+2=11)
    // 0->2->3 (9+11=20) or 0->1->3 (7+15=22) -> So 0->2->3 is shorter
    // 0->2->3->4 (20+6=26) or 0->2->5->4 (11+9=20) -> So 0->2->5->4 is shorter
    // print_paths("Dijkstra Complex", 0, 6, distances, predecessors);
    assert(distances[0] == 0);
    assert(distances[1] == 7 && predecessors[1] == 0);
    assert(distances[2] == 9 && predecessors[2] == 0);
    assert(distances[3] == 20 && predecessors[3] == 2);
    assert(distances[4] == 20 && predecessors[4] == 5);
    assert(distances[5] == 11 && predecessors[5] == 2);
    destroy_graph(g);
    printf("  test_dijkstra_more_complex: PASSED\n");
}

void test_dijkstra_negative_edge(void) {
    printf("  Testing Dijkstra: Graph with negative edge...\n");
    Graph* g = create_graph(3, true);
    add_edge(g, 0, 1, 1);
    add_edge(g, 1, 2, -5); // Negative edge
    int distances[3];
    int predecessors[3];
    initialize_paths(3, distances, predecessors);
    assert(dijkstra(g, 0, distances, predecessors) == -1); // Should return error
    destroy_graph(g);
    printf("  test_dijkstra_negative_edge: PASSED\n");
}

void test_dijkstra_disconnected(void) {
    printf("  Testing Dijkstra: Disconnected graph...\n");
    Graph* g = create_graph(5, false); 
    add_edge(g, 0, 1, 1); add_edge(g, 1, 2, 1);
    add_edge(g, 3, 4, 1);

    int distances[5];
    int predecessors[5];
    initialize_paths(5, distances, predecessors);
    assert(dijkstra(g, 0, distances, predecessors) == 0);
    assert(distances[0] == 0);
    assert(distances[1] == 1 && predecessors[1] == 0);
    assert(distances[2] == 2 && predecessors[2] == 1);
    assert(distances[3] == INT_MAX && predecessors[3] == -1);
    assert(distances[4] == INT_MAX && predecessors[4] == -1);
    destroy_graph(g);
    printf("  test_dijkstra_disconnected: PASSED\n");
}

void test_dijkstra_single_node(void) {
    printf("  Testing Dijkstra: Single node graph...\n");
    Graph* g = create_graph(1, false);
    int distances[1];
    int predecessors[1];
    initialize_paths(1, distances, predecessors);
    assert(dijkstra(g, 0, distances, predecessors) == 0);
    assert(distances[0] == 0 && predecessors[0] == -1);
    destroy_graph(g);
    printf("  test_dijkstra_single_node: PASSED\n");
}

void test_dijkstra_cyclic_graph_non_negative(void) {
    printf("  Testing Dijkstra: Cyclic graph with non-negative weights...\n");
    Graph* g = create_graph(3, false); 
    add_edge(g, 0, 1, 1); add_edge(g, 1, 2, 1); add_edge(g, 2, 0, 1); 
    
    int distances[3];
    int predecessors[3];
    initialize_paths(3, distances, predecessors);
    assert(dijkstra(g, 0, distances, predecessors) == 0);
    assert(distances[0] == 0);
    assert(distances[1] == 1 && predecessors[1] == 0);
    assert(distances[2] == 1 && predecessors[2] == 0); // Path 0->2 is shorter or equal to 0->1->2
    destroy_graph(g);
    printf("  test_dijkstra_cyclic_graph_non_negative: PASSED\n");
}

void test_dijkstra_all_vertices_reachable(void) {
    printf("  Testing Dijkstra: All vertices reachable...\n");
    Graph* g = create_graph(4, false);
    add_edge(g, 0, 1, 1); add_edge(g, 1, 2, 2); add_edge(g, 2, 3, 3);
    int distances[4];
    int predecessors[4];
    initialize_paths(4, distances, predecessors);
    assert(dijkstra(g, 0, distances, predecessors) == 0);
    for(int i=0; i<4; ++i) {
        assert(distances[i] != INT_MAX);
    }
    // print_paths("Dijkstra All Reachable",0,4,distances,predecessors);
    assert(distances[3] == 6); // 1+2+3
    destroy_graph(g);
    printf("  test_dijkstra_all_vertices_reachable: PASSED\n");
}

void test_dijkstra_empty_graph_and_out_of_bounds(void){
    printf("  Testing Dijkstra: Empty graph and out-of-bounds start vertex...\n");
    Graph* g_empty = create_graph(0, false);
    int dist_empty[1]; // Dummy, won't be used if num_vertices is 0
    int pred_empty[1];
    assert(dijkstra(g_empty, 0, dist_empty, pred_empty) == 0); // Start vertex 0 on 0-vertex graph is technically out of bounds, but Dijkstra handles num_vertices==0 first.
    destroy_graph(g_empty);

    Graph* g_valid = create_graph(3, false);
    int distances[3];
    int predecessors[3];
    assert(dijkstra(g_valid, 3, distances, predecessors) == -1); // Start vertex 3 out of bounds
    assert(dijkstra(g_valid, -1, distances, predecessors) == -1); // Start vertex -1 out of bounds
    destroy_graph(g_valid);
    printf("  test_dijkstra_empty_graph_and_out_of_bounds: PASSED\n");
}


// --- Bellman-Ford Algorithm Tests ---

void test_bellman_ford_simple(void) {
    printf("  Testing Bellman-Ford: Simple graph (same as Dijkstra simple)...\n");
    Graph* g = create_graph(5, false); 
    add_edge(g, 0, 1, 10); add_edge(g, 0, 2, 3);
    add_edge(g, 1, 2, 1);  add_edge(g, 1, 3, 2);
    add_edge(g, 2, 1, 4);  add_edge(g, 2, 3, 8); add_edge(g, 2, 4, 2);
    add_edge(g, 3, 4, 5);

    int distances[5];
    int predecessors[5];
    initialize_paths(5, distances, predecessors);
    assert(bellman_ford(g, 0, distances, predecessors) == 0); 
    assert(distances[0] == 0);
    assert(distances[1] == 4 && predecessors[1] == 2);
    assert(distances[2] == 3 && predecessors[2] == 0);
    assert(distances[3] == 6 && predecessors[3] == 1);
    assert(distances[4] == 5 && predecessors[4] == 2);
    destroy_graph(g);
    printf("  test_bellman_ford_simple: PASSED\n");
}

void test_bellman_ford_negative_weights_no_cycle(void) {
    printf("  Testing Bellman-Ford: Negative weights, no negative cycle...\n");
    Graph* g = create_graph(4, true); 
    add_edge(g, 0, 1, 5); add_edge(g, 0, 2, 4);
    add_edge(g, 1, 3, 3); add_edge(g, 2, 1, -6); 
    add_edge(g, 3, 2, 2);  

    int distances[4];
    int predecessors[4];
    initialize_paths(4, distances, predecessors);
    assert(bellman_ford(g, 0, distances, predecessors) == 0);
    assert(distances[0] == 0);
    assert(distances[1] == -2 && predecessors[1] == 2); // 0->2->1 = 4-6 = -2
    assert(distances[2] == 4 && predecessors[2] == 0);
    assert(distances[3] == 1 && predecessors[3] == 1); // 0->2->1->3 = 4-6+3 = 1
    destroy_graph(g);
    printf("  test_bellman_ford_negative_weights_no_cycle: PASSED\n");
}

void test_bellman_ford_another_negative_no_cycle(void) {
    printf("  Testing Bellman-Ford: Another negative weights, no negative cycle...\n");
    Graph* g = create_graph(5, true);
    add_edge(g, 0, 1, 1);
    add_edge(g, 1, 2, 2);
    add_edge(g, 1, 3, 5); // Path 0->1->3 = 6
    add_edge(g, 2, 3, -4); // Path 0->1->2->3 = 1+2-4 = -1
    add_edge(g, 3, 4, 3);  // Path 0->1->2->3->4 = -1+3 = 2
    add_edge(g, 0, 4, 3);  // Direct path 0->4 = 3. Longer.

    int distances[5];
    int predecessors[5];
    initialize_paths(5, distances, predecessors);
    assert(bellman_ford(g, 0, distances, predecessors) == 0);
    // print_paths("BF Another Neg No Cycle", 0, 5, distances, predecessors);
    assert(distances[0] == 0);
    assert(distances[1] == 1 && predecessors[1] == 0);
    assert(distances[2] == 3 && predecessors[2] == 1); // 0->1->2 = 1+2=3
    assert(distances[3] == -1 && predecessors[3] == 2); // 0->1->2->3 = 1+2-4 = -1
    assert(distances[4] == 2 && predecessors[4] == 3); // 0->1->2->3->4 = -1+3 = 2
    destroy_graph(g);
    printf("  test_bellman_ford_another_negative_no_cycle: PASSED\n");
}

void test_bellman_ford_detects_negative_cycle_simple(void) {
    printf("  Testing Bellman-Ford: Detects simple negative cycle (0->1(1), 1->0(-2))...\n");
    Graph* g = create_graph(2, true);
    add_edge(g, 0, 1, 1);
    add_edge(g, 1, 0, -2); // Cycle 0-1-0 with weight -1
    int distances[2];
    int predecessors[2];
    initialize_paths(2, distances, predecessors);
    assert(bellman_ford(g, 0, distances, predecessors) == 1); 
    destroy_graph(g);
    printf("  test_bellman_ford_detects_negative_cycle_simple: PASSED\n");
}

void test_bellman_ford_detects_negative_cycle_reachable(void) {
    printf("  Testing Bellman-Ford: Detects negative weight cycle reachable from start...\n");
    Graph* g = create_graph(4, true);
    add_edge(g, 0, 1, 1);    // Path to cycle
    add_edge(g, 1, 2, 2);
    add_edge(g, 2, 1, -3);   // Negative cycle 1-2-1 with weight -1
    add_edge(g, 2, 3, 1);    // Path from cycle

    int distances[4];
    int predecessors[4];
    initialize_paths(4, distances, predecessors);
    assert(bellman_ford(g, 0, distances, predecessors) == 1); 
    // print_paths("BF Neg Cycle Reachable", 0, 4, distances, predecessors);
    // Distances to nodes in/after cycle might be -INT_MAX or very small.
    destroy_graph(g);
    printf("  test_bellman_ford_detects_negative_cycle_reachable: PASSED\n");
}

void test_bellman_ford_detects_negative_cycle_unreachable(void) {
    printf("  Testing Bellman-Ford: Negative cycle unreachable from start...\n");
    Graph* g = create_graph(4, true);
    add_edge(g, 0, 3, 1);    // Path from start, does not reach cycle
    add_edge(g, 1, 2, 2);
    add_edge(g, 2, 1, -3);   // Negative cycle 1-2-1, unreachable from 0

    int distances[4];
    int predecessors[4];
    initialize_paths(4, distances, predecessors);
    // Standard Bellman-Ford from a source `s` only guarantees detection of negative cycles
    // reachable from `s`. If the cycle is unreachable, `distances` to nodes in that cycle
    // will remain INT_MAX, and the relaxation check for those edges won't trigger
    // the `distances[u] + w < distances[v]` condition in the V-th iteration if `distances[u]` is INT_MAX.
    // Thus, it should return 0 (no negative cycle *affecting paths from start* detected).
    assert(bellman_ford(g, 0, distances, predecessors) == 0); 
    assert(distances[0] == 0);
    assert(distances[3] == 1 && predecessors[3] == 0);
    assert(distances[1] == INT_MAX);
    assert(distances[2] == INT_MAX);
    // print_paths("BF Neg Cycle Unreachable",0,4,distances,predecessors);

    destroy_graph(g);
    printf("  test_bellman_ford_detects_negative_cycle_unreachable: PASSED\n");
}


void test_bellman_ford_no_path_to_negative_cycle(void) {
    // This is essentially the same as test_bellman_ford_detects_negative_cycle_unreachable
    // The Bellman-Ford as implemented will return 0 if the negative cycle is not reachable from start_vertex,
    // because the distances to nodes in that cycle will remain INT_MAX.
    printf("  Testing Bellman-Ford: No path from start to a negative cycle...\n");
    Graph* g = create_graph(5, true);
    add_edge(g, 0, 1, 10); // Path from start
    add_edge(g, 1, 4, 20);
    // Negative cycle component, not reachable from 0
    add_edge(g, 2, 3, -5);
    add_edge(g, 3, 2, 2);

    int distances[5];
    int predecessors[5];
    initialize_paths(5, distances, predecessors);
    assert(bellman_ford(g, 0, distances, predecessors) == 0);
    assert(distances[0] == 0);
    assert(distances[1] == 10);
    assert(distances[4] == 30);
    assert(distances[2] == INT_MAX);
    assert(distances[3] == INT_MAX);
    destroy_graph(g);
    printf("  test_bellman_ford_no_path_to_negative_cycle: PASSED\n");
}


void test_bellman_ford_disconnected(void) {
    printf("  Testing Bellman-Ford: Disconnected graph...\n");
    Graph* g = create_graph(5, false); 
    add_edge(g, 0, 1, 1); add_edge(g, 1, 2, 1);
    add_edge(g, 3, 4, 1);

    int distances[5];
    int predecessors[5];
    initialize_paths(5, distances, predecessors);
    assert(bellman_ford(g, 0, distances, predecessors) == 0);
    assert(distances[0] == 0);
    assert(distances[1] == 1 && predecessors[1] == 0);
    assert(distances[2] == 2 && predecessors[2] == 1);
    assert(distances[3] == INT_MAX && predecessors[3] == -1);
    assert(distances[4] == INT_MAX && predecessors[4] == -1);
    destroy_graph(g);
    printf("  test_bellman_ford_disconnected: PASSED\n");
}

void test_bellman_ford_single_node(void) {
    printf("  Testing Bellman-Ford: Single node graph...\n");
    Graph* g = create_graph(1, false);
    int distances[1];
    int predecessors[1];
    initialize_paths(1, distances, predecessors);
    assert(bellman_ford(g, 0, distances, predecessors) == 0);
    assert(distances[0] == 0 && predecessors[0] == -1);
    destroy_graph(g);
    printf("  test_bellman_ford_single_node: PASSED\n");
}

void test_bellman_ford_empty_graph_and_out_of_bounds(void) {
    printf("  Testing Bellman-Ford: Empty graph and out-of-bounds start vertex...\n");
    Graph* g_empty = create_graph(0, false);
    int dist_empty[1]; 
    int pred_empty[1];
    assert(bellman_ford(g_empty, 0, dist_empty, pred_empty) == 0); 
    destroy_graph(g_empty);

    Graph* g_valid = create_graph(3, false);
    int distances[3];
    int predecessors[3];
    assert(bellman_ford(g_valid, 3, distances, predecessors) == -1); 
    assert(bellman_ford(g_valid, -1, distances, predecessors) == -1); 
    destroy_graph(g_valid);
    printf("  test_bellman_ford_empty_graph_and_out_of_bounds: PASSED\n");
}
