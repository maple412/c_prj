#ifndef RBTREE_H
#define RBTREE_H

#include <stdio.h>
#include <stdlib.h> // For size_t, malloc, free
#include <stddef.h> // For NULL

typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    int data;
    Color color;
    struct RBNode *parent;
    struct RBNode *left;
    struct RBNode *right;
} RBNode;

// RBTree struct holds the root and the sentinel NIL node.
typedef struct {
    RBNode *root;
    RBNode *NIL;  // Sentinel node for all leaves and initial root parent
    size_t size;  // Number of internal nodes
} RBTree;

// Lifecycle
RBTree* create_rbtree(void);
void destroy_rbtree(RBTree* tree); // Frees all nodes including NIL if dynamically allocated per tree, and the tree struct

// Operations
int insert_rbnode(RBTree* tree, int data); // Returns 0:success, 1:exists, -1:error
int delete_rbnode(RBTree* tree, int data); // Returns 0:success, 1:not found
RBNode* search_rbnode(RBTree* tree, int data); // Returns node or tree->NIL if not found

// Traversals/Utilities
void inorder_rbtree_traversal(RBTree* tree, RBNode* node); // Pass tree->root to start
void print_rbtree_structure(RBTree* tree, RBNode* node, int level); // For debugging
size_t get_rbtree_size(RBTree* tree);

#endif // RBTREE_H
