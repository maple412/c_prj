# Comprehensive Data Structures and Algorithms in C

## Introduction

This project is a collection of C implementations of common data structures and algorithms. Each module is developed with a focus on clarity, correctness, and includes a dedicated test suite to ensure reliability. The primary purpose is educational and to serve as a reference for these fundamental computer science concepts.

## Implemented Modules

Below is a list of the data structures and algorithms implemented in this project.

---

### 1. Dynamic Array

*   **Description:** A resizable array that can grow or shrink in size as needed. Provides O(1) average time access to elements.
*   **Source Files:** `src/array.h`, `src/array.c`
*   **Test File:** `tests/test_array.c`
*   **Key Operations & Complexities:**
    *   Access: O(1)
    *   Search: O(n)
    *   Insertion (append): O(1) average, O(n) worst-case (due to resize)
    *   Insertion (at index): O(n) average/worst-case
    *   Deletion (pop): O(1) average, O(n) worst-case (due to resize)
    *   Deletion (at index): O(n) average/worst-case
    *   Space: O(n)

---

### 2. Singly Linked List

*   **Description:** A linear collection of data elements (nodes) where each node points to the next node in the sequence.
*   **Source Files:** `src/linked_list.h`, `src/linked_list.c`
*   **Test File:** `tests/test_linked_list.c`
*   **Key Operations & Complexities:**
    *   Access (k-th element): O(k)
    *   Search: O(n)
    *   Insertion (head): O(1)
    *   Insertion (tail): O(n) without tail pointer, O(1) with tail pointer (current impl: O(n))
    *   Insertion (middle, given predecessor): O(1)
    *   Deletion (head): O(1)
    *   Deletion (tail): O(n)
    *   Deletion (middle, given predecessor): O(1)
    *   Space: O(n)

---

### 3. Binary Search Tree (BST)

*   **Description:** A node-based binary tree data structure which has the property that the key in each node must be greater than or equal to any key stored in the left sub-tree, and less than or equal to any key stored in the right sub-tree. This implementation is not self-balancing.
*   **Source Files:** `src/binary_tree.h`, `src/binary_tree.c`
*   **Test File:** `tests/test_binary_tree.c`
*   **Key Operations & Complexities (Average / Worst for unbalanced):**
    *   Search: O(log n) / O(n)
    *   Insertion: O(log n) / O(n)
    *   Deletion: O(log n) / O(n)
    *   Space: O(n)

---

### 4. AVL Tree

*   **Description:** A self-balancing Binary Search Tree. The heights of the two child subtrees of any node differ by at most one. If at any time they differ by more than one, rebalancing (rotations) is done to restore this property.
*   **Source Files:** `src/avl_tree.h`, `src/avl_tree.c`
*   **Test File:** `tests/test_avl_tree.c`
*   **Key Operations & Complexities:**
    *   Search: O(log n)
    *   Insertion: O(log n) (includes rotations)
    *   Deletion: O(log n) (includes rotations)
    *   Space: O(n)

---

### 5. Red-Black Tree (RBT)

*   **Description:** A self-balancing Binary Search Tree where each node has an extra bit for storing color (red or black). Properties related to node colors and arrangements ensure the tree remains approximately balanced during insertions and deletions.
*   **Source Files:** `src/rbtree.h`, `src/rbtree.c`
*   **Test File:** `tests/test_rbtree.c`
*   **Key Operations & Complexities:**
    *   Search: O(log n)
    *   Insertion: O(log n)
    *   Deletion: O(log n)
    *   Space: O(n)

---

### 6. B-Tree

*   **Description:** A self-balancing tree data structure that maintains sorted data and allows searches, sequential access, insertions, and deletions in logarithmic time. It is optimized for systems that read and write large blocks of data. Each node can have multiple keys and multiple children. `t` is the minimum degree.
*   **Source Files:** `src/btree.h`, `src/btree.c`
*   **Test File:** `tests/test_btree.c`
*   **Key Operations & Complexities:**
    *   Search: O(log n) in terms of nodes, or O(t log_t n) for key comparisons.
    *   Insertion: O(log n) in terms of nodes, O(t log_t n) for key operations (splits can propagate).
    *   Deletion: O(log n) in terms of nodes, O(t log_t n) for key operations (merges/borrows can propagate).
    *   Space: O(n)

---

### 7. Huffman Coding

*   **Description:** An algorithm for lossless data compression. It assigns variable-length codes to input characters, lengths being based on the frequencies of corresponding characters.
*   **Source Files:** `src/huffman.h`, `src/huffman.c`
*   **Test File:** `tests/test_huffman.c`
*   **Key Operations & Complexities:**
    *   Building Huffman Tree: O(C log C) where C is the number of unique symbols (using a min-priority queue for C symbols).
    *   Generating Codes: O(C * H_avg) where H_avg is average height, or O(C * MaxCodeLen) if traversing. Typically O(C log C) or O(C) if tree structure allows.
    *   Encoding Data: O(N * L_avg) where N is the number of symbols in the input data and L_avg is the average Huffman code length.
    *   Decoding Data: O(M * H_max) where M is the number of bits in the encoded data and H_max is the maximum height of the Huffman tree. Can be O(M) on average if tree traversals are efficient.
    *   Space: O(C + H_max) for storing the tree and the codes.

---

### 8. Graph (Adjacency List Representation)

*   **Description:** Represents a graph using an array of adjacency lists. Supports directed and undirected graphs, with weighted edges.
*   **Source Files:** `src/graph.h`, `src/graph.c`
*   **Test File:** (Tested via `tests/test_graph_traversal.c` and `tests/test_shortest_path.c`)
*   **Key Operations & Complexities:**
    *   Add Vertex: O(1) (Current implementation initializes with a fixed number of vertices. Dynamic addition would be O(V) if resizing an underlying array of lists).
    *   Add Edge: O(1) (insertion at the head of the list).
    *   Space: O(V + E) where V is the number of vertices and E is the number of edges.

---

### 9. Graph Traversals (BFS, DFS)

*   **Description:** Algorithms for traversing or searching tree or graph data structures.
    *   BFS (Breadth-First Search): Explores neighbor nodes first, before moving to the next level neighbors.
    *   DFS (Depth-First Search): Explores as far as possible along each branch before backtracking.
*   **Source Files:** `src/graph_traversal.h`, `src/graph_traversal.c` (uses `src/graph.c`)
*   **Test File:** `tests/test_graph_traversal.c`
*   **Key Operations & Complexities (for Adjacency List representation):**
    *   Time (BFS & DFS): O(V + E)
    *   Space (BFS & DFS): O(V) (for queue/stack and visited array)

---

### 10. Shortest Path Algorithms

*   **Description:** Algorithms to find the shortest path between nodes in a graph.
    *   **Dijkstra's Algorithm:** Finds the shortest paths from a single source vertex to all other vertices in a weighted graph with non-negative edge weights.
    *   **Bellman-Ford Algorithm:** Finds shortest paths from a single source vertex to all other vertices in a weighted graph. Can handle negative edge weights and detect negative cycles.
*   **Source Files:** `src/shortest_path.h`, `src/shortest_path.c` (uses `src/graph.c`)
*   **Test File:** `tests/test_shortest_path.c`
*   **Key Operations & Complexities:**
    *   **Dijkstra:**
        *   Time: O((V+E)logV) with a binary heap based min-priority queue (as implemented). Could be O(E logV) if E is dominant and all nodes are reachable. O(V^2) with a simple array scan for min.
        *   Space: O(V) (for distances, predecessors, and priority queue).
    *   **Bellman-Ford:**
        *   Time: O(VE)
        *   Space: O(V) (for distances and predecessors).

---

## Compilation and Testing

A `Makefile` is provided in the root directory to compile and run the tests for all modules.

### Build All Test Executables

To compile all test executables, navigate to the project root directory and run:
```bash
make all
```
or simply:
```bash
make
```

### Run All Tests

After successful compilation, to run all tests:
```bash
make run_tests
```
This command will execute each compiled test program located in the `tests/` directory.

### Build a Specific Test

To compile a specific test module, use its target name. For example, to compile the test for the Dynamic Array:
```bash
make tests/test_array
```
Replace `test_array` with the name of the desired test executable (e.g., `test_linked_list`, `test_avl_tree`).

### Run a Specific Test

After compiling a specific test, you can run it directly:
```bash
./tests/test_array
```
Replace `./tests/test_array` with the path to the desired compiled test executable.

### Clean Up

To remove all compiled test executables and any other build artifacts:
```bash
make clean
```

## Project Structure

*   `src/`: Contains the source code (`.c`) and header files (`.h`) for each data structure and algorithm implementation.
*   `tests/`: Contains the test source files (`.c`) for each module. Compiled test executables will also be placed here by the `Makefile`.
*   `Makefile`: Build script for compiling and running tests.
*   `README.md`: This file.

## How to Use (Example)

To use a data structure, include its header file in your C program and link against the compiled source code (or compile it directly with your project).

Example for Dynamic Array:
```c
#include "src/array.h" // Adjust path if necessary
#include <stdio.h>

int main() {
    DynamicArray *my_array = create_array(10);
    if (my_array == NULL) {
        return 1;
    }

    append_element(my_array, 100);
    append_element(my_array, 200);

    print_array(my_array);
    // Expected output: Array (Size: 2, Capacity: 10): [100, 200]

    destroy_array(my_array);
    return 0;
}
```
Refer to the header files in the `src/` directory for the public API of each module.

## Contributing

This project is primarily for educational and demonstrative purposes. Contributions for bug fixes, improvements, or new data structures/algorithms are welcome. Please follow standard C coding practices and ensure any contributions include corresponding tests. (Standard placeholder text).
