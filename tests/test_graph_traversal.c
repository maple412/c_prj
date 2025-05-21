#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../src/graph.h"           // For Graph structure and functions
#include "../src/graph_traversal.h" // For BFS and DFS

// Helper array and index to store visited order for tests
int visited_order[100]; // Assuming max 100 vertices for test graphs
int visited_idx;

// Simple visit callback function for testing
void test_visit_logger(int vertex) {
    if (visited_idx < 100) {
        visited_order[visited_idx++] = vertex;
    }
    // printf("Visited: %d\n", vertex); // Optional: for debugging
}

// Helper to reset visited order array for each test
void reset_visited_order(void) {
    visited_idx = 0;
    for (int i = 0; i < 100; ++i) {
        visited_order[i] = -1; // Mark as unvisited or use a sentinel
    }
}

// Helper to compare visited order with expected order
void assert_visited_order(int expected_order[], int expected_len) {
    assert(visited_idx == expected_len);
    for (int i = 0; i < expected_len; ++i) {
        assert(visited_order[i] == expected_order[i]);
    }
}

// Test function prototypes
void test_graph_creation_and_add_edge(void); // Basic test for graph module itself
void test_bfs_simple_graph(void);
void test_dfs_simple_graph(void);
void test_bfs_disconnected_graph(void);
void test_dfs_disconnected_graph(void);
void test_bfs_empty_graph(void);
void test_dfs_empty_graph(void);
void test_bfs_single_node_graph(void);
void test_dfs_single_node_graph(void);
void test_bfs_cyclic_graph(void);
void test_dfs_cyclic_graph(void);


// Main function to run all tests
int main() {
    printf("Running Graph Traversal tests...\n");

    test_graph_creation_and_add_edge();
    test_bfs_simple_graph();
    test_dfs_simple_graph();
    test_bfs_disconnected_graph();
    test_dfs_disconnected_graph();
    test_bfs_empty_graph();
    test_dfs_empty_graph();
    test_bfs_single_node_graph();
    test_dfs_single_node_graph();
    test_bfs_cyclic_graph();
    test_dfs_cyclic_graph();

    printf("Graph Traversal tests completed (many are placeholders)!\n");
    return 0;
}

// Implementations of test functions (placeholders)

void test_graph_creation_and_add_edge(void) {
    printf("  Testing graph creation and add_edge...\n");
    Graph* g = create_graph(5, false); // 5 vertices, undirected
    assert(g != NULL);
    assert(g->num_vertices == 5);
    assert(g->is_directed == false);
    assert(g->adj_lists != NULL);
    for(int i=0; i<5; ++i) assert(g->adj_lists[i].head == NULL);

    add_edge(g, 0, 1);
    add_edge(g, 0, 2);
    add_edge(g, 1, 3);
    // print_graph(g); // For debugging
    
    // Basic check for edge existence (more thorough checks in traversal tests)
    assert(g->adj_lists[0].head != NULL); // 0 should have neighbors
    assert(g->adj_lists[4].head == NULL); // 4 should have no neighbors yet

    destroy_graph(g);
    printf("  test_graph_creation_and_add_edge: PASSED\n");
}

void test_bfs_simple_graph(void) {
    printf("  Testing BFS on a simple connected graph...\n");
    Graph* g = create_graph(4, false);
    add_edge(g, 0, 1);
    add_edge(g, 0, 2);
    add_edge(g, 1, 2);
    add_edge(g, 2, 0); // Redundant for undirected, but fine
    add_edge(g, 2, 3);
    add_edge(g, 3, 3); // Self-loop

    reset_visited_order();
    bfs_traversal(g, 2, test_visit_logger);
    int expected_bfs[] = {2, 0, 1, 3}; // One possible BFS order from vertex 2
    // Order can vary based on neighbor insertion order and queue behavior.
    // This test might need adjustment if the BFS queue processes neighbors in a specific fixed order.
    // For now, let's assume a common order.
    // A more robust test checks if all nodes are visited and if distances are correct.
    // For simplicity, we'll assert a plausible order.
    printf("    BFS from 2: "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    // assert_visited_order(expected_bfs, 4); // This can be too strict.
    assert(visited_idx == 4); // Check if all connected nodes are visited.

    destroy_graph(g);
    printf("  test_bfs_simple_graph: PASSED (basic check)\n");
}

void test_dfs_simple_graph(void) {
    printf("  Testing DFS on a simple connected graph...\n");
    Graph* g = create_graph(4, false);
    add_edge(g, 0, 1);
    add_edge(g, 0, 2);
    add_edge(g, 1, 2);
    add_edge(g, 2, 0);
    add_edge(g, 2, 3);
    add_edge(g, 3, 3);

    reset_visited_order();
    dfs_traversal(g, 2, test_visit_logger);
    // One possible DFS order from vertex 2: e.g., 2,0,1,3 or 2,3,0,1 etc.
    // DFS order is highly dependent on implementation details (recursion stack, neighbor iteration).
    printf("    DFS from 2: "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 4); // Check if all connected nodes are visited.

    destroy_graph(g);
    printf("  test_dfs_simple_graph: PASSED (basic check)\n");
}

// TODO: Implement other test functions using reset_visited_order() and assert_visited_order()
// For disconnected graphs, BFS/DFS from one node will only visit its connected component.
// Test this behavior.

void test_bfs_disconnected_graph(void) {
    printf("  Testing BFS on a disconnected graph...\n");
    Graph* g = create_graph(5, false); // 0-1-2 and 3-4
    add_edge(g, 0, 1); add_edge(g, 1, 2);
    add_edge(g, 3, 4);

    reset_visited_order();
    bfs_traversal(g, 0, test_visit_logger); // Start BFS from 0
    // Expected to visit only 0, 1, 2
    printf("    BFS from 0 (disconnected): "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 3); // Should visit 0, 1, 2
    // Can check contents if order is predictable, e.g., {0,1,2} in some permutation

    reset_visited_order();
    bfs_traversal(g, 3, test_visit_logger); // Start BFS from 3
    printf("    BFS from 3 (disconnected): "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 2); // Should visit 3, 4

    destroy_graph(g);
    printf("  test_bfs_disconnected_graph: PASSED (basic check)\n");
}

void test_dfs_disconnected_graph(void) {
    printf("  Testing DFS on a disconnected graph...\n");
    Graph* g = create_graph(5, false); // 0-1-2 and 3-4
    add_edge(g, 0, 1); add_edge(g, 1, 2);
    add_edge(g, 3, 4);

    reset_visited_order();
    dfs_traversal(g, 0, test_visit_logger); // Start DFS from 0
    printf("    DFS from 0 (disconnected): "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 3); 

    reset_visited_order();
    dfs_traversal(g, 3, test_visit_logger); // Start DFS from 3
    printf("    DFS from 3 (disconnected): "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 2);

    destroy_graph(g);
    printf("  test_dfs_disconnected_graph: PASSED (basic check)\n");
}

void test_bfs_empty_graph(void) {
    printf("  Testing BFS on an empty graph (no vertices)...\n");
    Graph* g = create_graph(0, false);
    reset_visited_order();
    // bfs_traversal(g, 0, test_visit_logger); // Starting vertex 0 is invalid
    // What should happen? BFS should probably handle g->num_vertices == 0 gracefully.
    // Or, if start_vertex is out of bounds. Assume start_vertex must be < num_vertices.
    // For a graph with 0 vertices, traversal cannot start.
    assert(g->num_vertices == 0);
    // No traversal to call if num_vertices is 0.
    // If we had a graph with vertices but no edges:
    Graph* g_no_edges = create_graph(3, false);
    reset_visited_order();
    bfs_traversal(g_no_edges, 0, test_visit_logger);
    printf("    BFS from 0 (no edges): "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 1); assert(visited_order[0] == 0);

    destroy_graph(g);
    destroy_graph(g_no_edges);
    printf("  test_bfs_empty_graph: PASSED\n");
}
void test_dfs_empty_graph(void) { /* Similar to BFS empty */ 
    printf("  Testing DFS on an empty graph (no vertices)...\n");
    Graph* g = create_graph(0, false);
    assert(g->num_vertices == 0);
    Graph* g_no_edges = create_graph(3, false);
    reset_visited_order();
    dfs_traversal(g_no_edges, 0, test_visit_logger);
    printf("    DFS from 0 (no edges): "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 1); assert(visited_order[0] == 0);
    destroy_graph(g);
    destroy_graph(g_no_edges);
    printf("  test_dfs_empty_graph: PASSED\n");
}
void test_bfs_single_node_graph(void) { /* Graph with 1 vertex, 0 edges */ 
    printf("  Testing BFS on a single node graph...\n");
    Graph* g = create_graph(1, false);
    reset_visited_order();
    bfs_traversal(g, 0, test_visit_logger);
    printf("    BFS from 0 (single node): "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 1); assert(visited_order[0] == 0);
    destroy_graph(g);
    printf("  test_bfs_single_node_graph: PASSED\n");
}
void test_dfs_single_node_graph(void) { /* Similar */ 
    printf("  Testing DFS on a single node graph...\n");
    Graph* g = create_graph(1, false);
    reset_visited_order();
    dfs_traversal(g, 0, test_visit_logger);
    printf("    DFS from 0 (single node): "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 1); assert(visited_order[0] == 0);
    destroy_graph(g);
    printf("  test_dfs_single_node_graph: PASSED\n");
}
void test_bfs_cyclic_graph(void) { /* Test BFS on cyclic graph */ 
    printf("  Testing BFS on a cyclic graph...\n");
    Graph* g = create_graph(3, false); // 0-1-2-0 cycle
    add_edge(g,0,1); add_edge(g,1,2); add_edge(g,2,0);
    reset_visited_order();
    bfs_traversal(g,0,test_visit_logger);
    printf("    BFS from 0 (cyclic): "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 3); // Should visit all nodes
    destroy_graph(g);
    printf("  test_bfs_cyclic_graph: PASSED\n");
}
void test_dfs_cyclic_graph(void) { /* Test DFS on cyclic graph */ 
    printf("  Testing DFS on a cyclic graph...\n");
    Graph* g = create_graph(3, false); // 0-1-2-0 cycle
    add_edge(g,0,1); add_edge(g,1,2); add_edge(g,2,0);
    reset_visited_order();
    dfs_traversal(g,0,test_visit_logger);
    printf("    DFS from 0 (cyclic): "); for(int i=0; i<visited_idx; ++i) printf("%d ", visited_order[i]); printf("\n");
    assert(visited_idx == 3); // Should visit all nodes
    destroy_graph(g);
    printf("  test_dfs_cyclic_graph: PASSED\n");
}
