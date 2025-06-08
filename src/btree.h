#ifndef BTREE_H
#define BTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // For bool type

// B-Tree Node Structure
typedef struct BTreeNode {
    int *keys;          // Array of keys
    struct BTreeNode **children; // Array of child pointers
    int num_keys;       // Current number of keys
    bool is_leaf;       // True if this node is a leaf
    // int t;           // Min degree, stored in BTree struct for access by helpers
} BTreeNode;

// B-Tree Structure
typedef struct {
    BTreeNode *root;    // Pointer to the root node
    int t;              // Minimum degree: Max keys = 2t-1, Min keys = t-1 (except root)
} BTree;

// Lifecycle functions
BTree* create_btree(int t);
void destroy_btree(BTree* tree);

// Operations
void insert_btree(BTree* tree, int key);
bool search_btree(BTree* tree, int key); // Returns true if key is found, false otherwise
void delete_btree(BTree* tree, int key); // Placeholder for complex delete

// Utility / Debugging
void print_btree(BTree* tree); // Prints the B-Tree structure

#endif // BTREE_H
