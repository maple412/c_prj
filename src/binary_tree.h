#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <stdio.h>
#include <stdlib.h> // For size_t, malloc, free

typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

// Optional: Wrapper for the tree, can hold root and size
typedef struct {
    TreeNode *root;
    size_t size;
} BinaryTree;

// Function Prototypes

// Tree lifecycle
BinaryTree* create_binary_tree(void);
void destroy_binary_tree(BinaryTree* tree); // Frees all nodes and the tree struct

// Binary Search Tree (BST) operations
// insert_bst will modify tree->root and tree->size
int insert_bst_node(BinaryTree* tree, int data);
// delete_bst will modify tree->root and tree->size
int delete_bst_node(BinaryTree* tree, int data);
TreeNode* search_bst_node(BinaryTree* tree, int data); // Searches from tree->root

// Traversals (can be called with tree->root)
void inorder_traversal(TreeNode* node);
void preorder_traversal(TreeNode* node);
void postorder_traversal(TreeNode* node);

// Utility functions
int get_tree_height(TreeNode* node); // Height of the subtree rooted at 'node'
TreeNode* find_min_node(TreeNode* node); // Finds min node in subtree rooted at 'node'
TreeNode* find_max_node(TreeNode* node); // Finds max node in subtree rooted at 'node'
size_t get_tree_size(BinaryTree* tree); // Returns tree->size

#endif // BINARY_TREE_H
