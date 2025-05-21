#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdio.h>
#include <stdlib.h> // For size_t, malloc, free
#include <stddef.h> // For NULL

// AVL Tree Node Structure
typedef struct AVLNode {
    int key;
    struct AVLNode *left;
    struct AVLNode *right;
    int height; // Height of this node in the tree
} AVLNode;

// AVLTree struct (optional wrapper for root and size)
typedef struct {
    AVLNode *root;
    size_t size;
} AVLTree;

// Lifecycle
AVLTree* create_avl_tree(void);
void destroy_avl_tree(AVLTree* tree); // Frees all nodes and the tree struct

// Core Operations
// These functions will manage tree->root and tree->size internally.
// Return 0 for success, 1 if key exists (for insert), 1 if key not found (for delete), -1 for error.
int insert_into_avl_tree(AVLTree* tree, int key);
int delete_from_avl_tree(AVLTree* tree, int key);
AVLNode* search_in_avl_tree(AVLTree* tree, int key); // Returns node if found, else NULL

// Traversals & Utilities
void inorder_avl_traversal(AVLNode* node); // Call with tree->root
void preorder_avl_traversal(AVLNode* node); // Optional
void postorder_avl_traversal(AVLNode* node); // Optional
size_t get_avl_tree_size(AVLTree* tree);
int get_avl_tree_height(AVLTree* tree); // Height of the entire tree

#endif // AVL_TREE_H
