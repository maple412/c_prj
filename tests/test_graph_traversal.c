#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h> // For memset

#include "../src/graph.h"           
#include "../src/graph_traversal.h" 

// Helper array and index to store visited order for tests
#define MAX_TEST_VERTICES 100
int visited_order[MAX_TEST_VERTICES]; 
int visited_idx;

// Simple visit callback function for testing
void test_visit_logger(int vertex) {
    if (visited_idx < MAX_TEST_VERTICES) {
        visited_order[visited_idx++] = vertex;
    }
}

// Helper to reset visited order array for each test
void reset_visited_order(void) {
    visited_idx = 0;
    for (int i = 0; i < MAX_TEST_VERTICES; ++i) {
        visited_order[i] = -1; 
    }
}

// Helper to sort an array (for comparing visited nodes irrespective of order)
static int compare_ints(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// Helper to compare visited order with expected order
// If check_exact_order is true, compares exact sequence.
// Otherwise, checks if the set of visited nodes is the same.
void assert_visited_nodes(int expected_nodes[], int expected_len, bool check_exact_order) {
    assert(visited_idx == expected_len);
    if (expected_len == 0) return; // Nothing to compare if no nodes were expected/visited

    if (check_exact_order) {
        for (int i = 0; i < expected_len; ++i) {
            assert(visited_order[i] == expected_nodes[i]);
        }
    } else {
        // Sort both arrays and then compare
        qsort(visited_order, visited_idx, sizeof(int), compare_ints);
        // expected_nodes might be modified by qsort, so make a copy if it's a literal
        int* sorted_expected = (int*)malloc(expected_len * sizeof(int));
        assert(sorted_expected != NULL);
        memcpy(sorted_expected, expected_nodes, expected_len * sizeof(int));
        qsort(sorted_expected, expected_len, sizeof(int), compare_ints);

        for (int i = 0; i < expected_len; ++i) {
            assert(visited_order[i] == sorted_expected[i]);
        }
        free(sorted_expected);
    }
}


// Test function prototypes
void test_graph_creation_and_add_edge(void);
void test_bfs_simple_graph(void);
void test_dfs_simple_graph(void);
void test_bfs_disconnected_graph(void);
void test_dfs_disconnected_graph(void);
void test_bfs_empty_graph_and_no_edges(void);
void test_dfs_empty_graph_and_no_edges(void);
void test_bfs_single_node_graph(void);
void test_dfs_single_node_graph(void);
void test_bfs_cyclic_graph_with_tail(void);
void test_dfs_cyclic_graph_with_tail(void);
void test_bfs_directed_graph(void);
void test_dfs_directed_graph(void);
void test_traversal_start_node_out_of_bounds(void);


// Main function to run all tests
int main() {
    printf("Running Graph Traversal tests...\n");

    test_graph_creation_and_add_edge();
    test_bfs_simple_graph();
    test_dfs_simple_graph();
    test_bfs_disconnected_graph();
    test_dfs_disconnected_graph();
    test_bfs_empty_graph_and_no_edges();
    test_dfs_empty_graph_and_no_edges();
    test_bfs_single_node_graph();
    test_dfs_single_node_graph();
    test_bfs_cyclic_graph_with_tail();
    test_dfs_cyclic_graph_with_tail();
    test_bfs_directed_graph();
    test_dfs_directed_graph();
    test_traversal_start_node_out_of_bounds();

    printf("\nGraph Traversal tests completed!\n");
    return 0;
}

void test_graph_creation_and_add_edge(void) {
    printf("  Testing graph creation and add_edge...\n");
    Graph* g = create_graph(5, false); 
    assert(g != NULL);
    assert(g->num_vertices == 5);
    assert(g->is_directed == false);
    assert(g->adj_lists != NULL);
    for(int i=0; i<5; ++i) assert(g->adj_lists[i].head == NULL);

    add_edge(g, 0, 1); // Edges are added to front
    add_edge(g, 0, 2);
    add_edge(g, 1, 3);
    
    assert(g->adj_lists[0].head != NULL && (g->adj_lists[0].head->dest_vertex == 2 || g->adj_lists[0].head->dest_vertex == 1) );
    assert(g->adj_lists[0].head->next != NULL && (g->adj_lists[0].head->next->dest_vertex == 1 || g->adj_lists[0].head->next->dest_vertex == 2));
    assert(g->adj_lists[1].head != NULL && (g->adj_lists[1].head->dest_vertex == 3 || g->adj_lists[1].head->dest_vertex == 0)); // Undirected
    assert(g->adj_lists[2].head != NULL && g->adj_lists[2].head->dest_vertex == 0); // Undirected
    assert(g->adj_lists[3].head != NULL && g->adj_lists[3].head->dest_vertex == 1); // Undirected
    assert(g->adj_lists[4].head == NULL); 

    destroy_graph(g);
    printf("  test_graph_creation_and_add_edge: PASSED\n");
}

void test_bfs_simple_graph(void) {
    printf("  Testing BFS on a simple connected graph (7 vertices)...\n");
    Graph* g = create_graph(7, false);
    //  0 -- 1 -- 2
    //  |    |    |
    //  3 -- 4 -- 5 -- 6
    add_edge(g, 0, 1); add_edge(g, 0, 3);
    add_edge(g, 1, 2); add_edge(g, 1, 4);
    add_edge(g, 2, 5);
    add_edge(g, 3, 4);
    add_edge(g, 4, 5);
    add_edge(g, 5, 6);

    reset_visited_order();
    bfs_traversal(g, 0, test_visit_logger);
    int expected_nodes[] = {0, 1, 3, 2, 4, 5, 6}; // All nodes
    assert_visited_nodes(expected_nodes, 7, false); // Order not strictly checked

    destroy_graph(g);
    printf("  test_bfs_simple_graph: PASSED\n");
}

void test_dfs_simple_graph(void) {
    printf("  Testing DFS on a simple connected graph (7 vertices)...\n");
    Graph* g = create_graph(7, false);
    add_edge(g, 0, 1); add_edge(g, 0, 3);
    add_edge(g, 1, 2); add_edge(g, 1, 4);
    add_edge(g, 2, 5);
    add_edge(g, 3, 4);
    add_edge(g, 4, 5);
    add_edge(g, 5, 6);

    reset_visited_order();
    dfs_traversal(g, 0, test_visit_logger);
    int expected_nodes[] = {0, 1, 2, 5, 6, 4, 3}; // All nodes
    assert_visited_nodes(expected_nodes, 7, false); // Order not strictly checked

    destroy_graph(g);
    printf("  test_dfs_simple_graph: PASSED\n");
}


void test_bfs_disconnected_graph(void) {
    printf("  Testing BFS on a disconnected graph...\n");
    Graph* g = create_graph(5, false); 
    add_edge(g, 0, 1); add_edge(g, 1, 2); // Component 1: 0,1,2
    add_edge(g, 3, 4);                   // Component 2: 3,4

    reset_visited_order();
    bfs_traversal(g, 0, test_visit_logger); 
    int comp1_nodes[] = {0, 1, 2};
    assert_visited_nodes(comp1_nodes, 3, false);

    reset_visited_order();
    bfs_traversal(g, 3, test_visit_logger); 
    int comp2_nodes[] = {3, 4};
    assert_visited_nodes(comp2_nodes, 2, false);

    destroy_graph(g);
    printf("  test_bfs_disconnected_graph: PASSED\n");
}

void test_dfs_disconnected_graph(void) {
    printf("  Testing DFS on a disconnected graph...\n");
    Graph* g = create_graph(5, false); 
    add_edge(g, 0, 1); add_edge(g, 1, 2);
    add_edge(g, 3, 4);

    reset_visited_order();
    dfs_traversal(g, 0, test_visit_logger);
    int comp1_nodes[] = {0, 1, 2};
    assert_visited_nodes(comp1_nodes, 3, false);

    reset_visited_order();
    dfs_traversal(g, 3, test_visit_logger);
    int comp2_nodes[] = {3, 4};
    assert_visited_nodes(comp2_nodes, 2, false);

    destroy_graph(g);
    printf("  test_dfs_disconnected_graph: PASSED\n");
}

void test_bfs_empty_graph_and_no_edges(void) {
    printf("  Testing BFS on an empty graph and graph with no edges...\n");
    Graph* g_empty = create_graph(0, false); // Graph with 0 vertices
    reset_visited_order();
    bfs_traversal(g_empty, 0, test_visit_logger); // Call with invalid start_vertex
    assert(visited_idx == 0); // Should do nothing
    destroy_graph(g_empty);

    Graph* g_no_edges = create_graph(3, false); // 3 vertices, 0 edges
    reset_visited_order();
    bfs_traversal(g_no_edges, 0, test_visit_logger);
    int expected_no_edges[] = {0};
    assert_visited_nodes(expected_no_edges, 1, true);
    destroy_graph(g_no_edges);
    printf("  test_bfs_empty_graph_and_no_edges: PASSED\n");
}

void test_dfs_empty_graph_and_no_edges(void) { 
    printf("  Testing DFS on an empty graph and graph with no edges...\n");
    Graph* g_empty = create_graph(0, false);
    reset_visited_order();
    dfs_traversal(g_empty, 0, test_visit_logger);
    assert(visited_idx == 0);
    destroy_graph(g_empty);

    Graph* g_no_edges = create_graph(3, false);
    reset_visited_order();
    dfs_traversal(g_no_edges, 1, test_visit_logger);
    int expected_no_edges[] = {1};
    assert_visited_nodes(expected_no_edges, 1, true);
    destroy_graph(g_no_edges);
    printf("  test_dfs_empty_graph_and_no_edges: PASSED\n");
}

void test_bfs_single_node_graph(void) { 
    printf("  Testing BFS on a single node graph...\n");
    Graph* g = create_graph(1, false);
    reset_visited_order();
    bfs_traversal(g, 0, test_visit_logger);
    int expected[] = {0};
    assert_visited_nodes(expected, 1, true);
    destroy_graph(g);
    printf("  test_bfs_single_node_graph: PASSED\n");
}

void test_dfs_single_node_graph(void) { 
    printf("  Testing DFS on a single node graph...\n");
    Graph* g = create_graph(1, false);
    reset_visited_order();
    dfs_traversal(g, 0, test_visit_logger);
    int expected[] = {0};
    assert_visited_nodes(expected, 1, true);
    destroy_graph(g);
    printf("  test_dfs_single_node_graph: PASSED\n");
}

void test_bfs_cyclic_graph_with_tail(void) { 
    printf("  Testing BFS on a cyclic graph with a tail...\n");
    Graph* g = create_graph(4, false); // 0-1, 1-2, 2-0 (cycle), 2-3 (tail)
    add_edge(g,0,1); add_edge(g,1,2); add_edge(g,2,0); add_edge(g,2,3);
    
    reset_visited_order();
    bfs_traversal(g,0,test_visit_logger);
    int expected_from_0[] = {0,1,2,3}; // All nodes
    assert_visited_nodes(expected_from_0, 4, false);

    reset_visited_order();
    bfs_traversal(g,3,test_visit_logger); // Start from tail
    int expected_from_3[] = {3,2,0,1}; // All nodes (since undirected)
    assert_visited_nodes(expected_from_3, 4, false);

    destroy_graph(g);
    printf("  test_bfs_cyclic_graph_with_tail: PASSED\n");
}

void test_dfs_cyclic_graph_with_tail(void) { 
    printf("  Testing DFS on a cyclic graph with a tail...\n");
    Graph* g = create_graph(4, false); 
    add_edge(g,0,1); add_edge(g,1,2); add_edge(g,2,0); add_edge(g,2,3);
    
    reset_visited_order();
    dfs_traversal(g,0,test_visit_logger);
    int expected_from_0[] = {0,1,2,3};
    assert_visited_nodes(expected_from_0, 4, false);

    reset_visited_order();
    dfs_traversal(g,3,test_visit_logger);
    int expected_from_3[] = {3,2,0,1}; 
    assert_visited_nodes(expected_from_3, 4, false);

    destroy_graph(g);
    printf("  test_dfs_cyclic_graph_with_tail: PASSED\n");
}

void test_bfs_directed_graph(void) {
    printf("  Testing BFS on a directed graph...\n");
    Graph* g = create_graph(4, true); // Directed
    add_edge(g, 0, 1); // 0 -> 1
    add_edge(g, 0, 2); // 0 -> 2
    add_edge(g, 1, 2); // 1 -> 2
    add_edge(g, 2, 3); // 2 -> 3

    reset_visited_order();
    bfs_traversal(g, 0, test_visit_logger); // Start from 0
    int expected_from_0[] = {0, 1, 2, 3}; // All reachable from 0
    assert_visited_nodes(expected_from_0, 4, false);

    reset_visited_order();
    bfs_traversal(g, 1, test_visit_logger); // Start from 1
    int expected_from_1[] = {1, 2, 3}; // Reachable from 1
    assert_visited_nodes(expected_from_1, 3, false);
    
    reset_visited_order();
    bfs_traversal(g, 3, test_visit_logger); // Start from 3
    int expected_from_3[] = {3}; // Only 3 is reachable from 3
    assert_visited_nodes(expected_from_3, 1, true);

    destroy_graph(g);
    printf("  test_bfs_directed_graph: PASSED\n");
}

void test_dfs_directed_graph(void) {
    printf("  Testing DFS on a directed graph...\n");
    Graph* g = create_graph(4, true);
    add_edge(g, 0, 1); add_edge(g, 0, 2); 
    add_edge(g, 1, 2); add_edge(g, 2, 3);

    reset_visited_order();
    dfs_traversal(g, 0, test_visit_logger); 
    int expected_from_0[] = {0, 1, 2, 3}; 
    assert_visited_nodes(expected_from_0, 4, false);

    reset_visited_order();
    dfs_traversal(g, 1, test_visit_logger); 
    int expected_from_1[] = {1, 2, 3}; 
    assert_visited_nodes(expected_from_1, 3, false);

    reset_visited_order();
    dfs_traversal(g, 3, test_visit_logger); 
    int expected_from_3[] = {3}; 
    assert_visited_nodes(expected_from_3, 1, true);

    destroy_graph(g);
    printf("  test_dfs_directed_graph: PASSED\n");
}

void test_traversal_start_node_out_of_bounds(void) {
    printf("  Testing traversals with start_vertex out of bounds...\n");
    Graph* g = create_graph(3, false);
    add_edge(g,0,1);

    reset_visited_order();
    bfs_traversal(g, 3, test_visit_logger); // Vertex 3 is out of bounds (0-2)
    assert(visited_idx == 0);

    reset_visited_order();
    bfs_traversal(g, -1, test_visit_logger); // Vertex -1 is out of bounds
    assert(visited_idx == 0);

    reset_visited_order();
    dfs_traversal(g, 3, test_visit_logger);
    assert(visited_idx == 0);

    reset_visited_order();
    dfs_traversal(g, -1, test_visit_logger);
    assert(visited_idx == 0);
    
    destroy_graph(g);
    printf("  test_traversal_start_node_out_of_bounds: PASSED\n");
}
