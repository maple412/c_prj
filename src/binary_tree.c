#include "binary_tree.h"
#include <stdio.h>
#include <stdlib.h>

// Static helper function prototypes
static TreeNode* create_new_tree_node(int data);
static void destroy_nodes_recursive(TreeNode* node);
static TreeNode* insert_recursive(TreeNode* node, int data, size_t* current_size_ptr, int* op_status_ptr);
static TreeNode* delete_recursive(TreeNode* node, int data, int* success_flag, size_t* current_size_ptr);
static TreeNode* find_min_recursive(TreeNode* node); // Used by delete_recursive
// find_max_recursive is not strictly needed if find_max_node is iterative.

// Helper to create a new tree node
static TreeNode* create_new_tree_node(int data) {
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    if (newNode == NULL) {
        perror("Failed to allocate memory for TreeNode");
        return NULL;
    }
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Tree lifecycle
BinaryTree* create_binary_tree(void) {
    BinaryTree* tree = (BinaryTree*)malloc(sizeof(BinaryTree));
    if (tree == NULL) {
        perror("Failed to allocate memory for BinaryTree");
        return NULL;
    }
    tree->root = NULL;
    tree->size = 0;
    return tree;
}

static void destroy_nodes_recursive(TreeNode* node) {
    if (node == NULL) {
        return;
    }
    destroy_nodes_recursive(node->left);
    destroy_nodes_recursive(node->right);
    free(node);
}

void destroy_binary_tree(BinaryTree* tree) {
    if (tree == NULL) {
        return;
    }
    destroy_nodes_recursive(tree->root);
    free(tree);
}

// Recursive helper for insert_bst_node
// op_status_ptr: 0 for inserted, 1 for exists, -1 for malloc error
static TreeNode* insert_recursive(TreeNode* node, int data, size_t* current_size_ptr, int* op_status_ptr) {
    if (node == NULL) {
        TreeNode* newNode = create_new_tree_node(data);
        if (newNode == NULL) {
            *op_status_ptr = -1; // Malloc error
            return NULL;
        }
        (*current_size_ptr)++;
        *op_status_ptr = 0; // Successfully inserted
        return newNode;
    }

    if (data < node->data) {
        node->left = insert_recursive(node->left, data, current_size_ptr, op_status_ptr);
    } else if (data > node->data) {
        node->right = insert_recursive(node->right, data, current_size_ptr, op_status_ptr);
    } else { // data == node->data
        *op_status_ptr = 1; // Value already exists
    }
    return node;
}

// Binary Search Tree (BST) operations
int insert_bst_node(BinaryTree* tree, int data) {
    if (tree == NULL) {
        return -1; // Error: NULL tree
    }
    int op_status = 0; // Default to success, will be updated by insert_recursive
    
    // No need to search first, insert_recursive will handle existing values and update op_status.
    // size_t original_size = tree->size; // Not needed with op_status from recursive call

    tree->root = insert_recursive(tree->root, data, &(tree->size), &op_status);

    // if tree->root became NULL AND op_status is -1, it means the very first node creation failed.
    if (tree->root == NULL && tree->size == 0 && op_status == -1) {
         // This means create_new_tree_node failed for the root.
         // tree->size would not have been incremented by insert_recursive if newNode was NULL.
         // op_status already reflects -1 (malloc error)
    }
    
    return op_status; // 0 for inserted, 1 for exists, -1 for malloc error
}


static TreeNode* find_min_recursive(TreeNode* node) {
    if (node == NULL) {
        return NULL;
    }
    if (node->left == NULL) {
        return node;
    }
    return find_min_recursive(node->left);
}

// success_flag: 1 if deleted, 0 if not found
// current_size_ptr: pointer to tree->size to decrement it
static TreeNode* delete_recursive(TreeNode* node, int data, int* success_flag, size_t* current_size_ptr) {
    if (node == NULL) {
        *success_flag = 0; // Node not found
        return NULL;
    }

    if (data < node->data) {
        node->left = delete_recursive(node->left, data, success_flag, current_size_ptr);
    } else if (data > node->data) {
        node->right = delete_recursive(node->right, data, success_flag, current_size_ptr);
    } else { // Node with data found
        *success_flag = 1;
        // Case 1: No child or only one child
        if (node->left == NULL) {
            TreeNode* temp = node->right;
            free(node);
            (*current_size_ptr)--;
            return temp;
        } else if (node->right == NULL) {
            TreeNode* temp = node->left;
            free(node);
            (*current_size_ptr)--;
            return temp;
        }

        // Case 2: Node with two children
        // Get the inorder successor (smallest in the right subtree)
        TreeNode* temp = find_min_recursive(node->right);
        node->data = temp->data; // Copy the inorder successor's content to this node
        // Delete the inorder successor. Note: success_flag for this sub-deletion is not the main one.
        // The size is already decremented effectively by replacing this node's data and deleting successor.
        // However, the recursive call to delete needs a flag. Let's use a dummy.
        int dummy_flag_for_successor_delete = 0; 
        // We don't decrement size here, it was done when we marked this node for deletion.
        // The recursive delete below will handle the actual node removal of the successor.
        // Crucially, the size should only be decremented ONCE per successful public delete_bst_node call.
        // The current_size_ptr is passed down, so if the successor is deleted, it would decrement.
        // This suggests the success_flag and size update logic needs to be precise.
        // Let's adjust: the main success_flag is set when the node is found. Size is decremented then.
        // The recursive calls just maintain the tree structure.
        node->right = delete_recursive(node->right, temp->data, &dummy_flag_for_successor_delete, current_size_ptr);
        // If dummy_flag_for_successor_delete is 0, it means temp->data wasn't found in right subtree, which is an error.
        // However, find_min_recursive guarantees temp is from right subtree.
        // The size was decremented when the original node was marked for deletion.
    }
    return node;
}


int delete_bst_node(BinaryTree* tree, int data) {
    if (tree == NULL || tree->root == NULL) {
        return 1; // Node not found (or tree empty) - return 1 as per previous logic
    }
    int success_flag = 0;
    // tree->size is passed by reference and decremented inside delete_recursive when node is freed
    tree->root = delete_recursive(tree->root, data, &success_flag, &(tree->size));

    if (success_flag) {
        return 0; // Successfully deleted
    } else {
        return 1; // Node not found
    }
}

// Recursive helper for search_bst_node
static TreeNode* search_recursive(TreeNode* node, int data) {
    if (node == NULL || node->data == data) {
        return node;
    }
    if (data < node->data) {
        return search_recursive(node->left, data);
    } else {
        return search_recursive(node->right, data);
    }
}

TreeNode* search_bst_node(BinaryTree* tree, int data) {
    if (tree == NULL) {
        return NULL;
    }
    return search_recursive(tree->root, data);
}

// Traversals
void inorder_traversal(TreeNode* node) {
    if (node == NULL) {
        return;
    }
    inorder_traversal(node->left);
    printf("%d ", node->data);
    inorder_traversal(node->right);
}

void preorder_traversal(TreeNode* node) {
    if (node == NULL) {
        return;
    }
    printf("%d ", node->data);
    preorder_traversal(node->left);
    preorder_traversal(node->right);
}

void postorder_traversal(TreeNode* node) {
    if (node == NULL) {
        return;
    }
    postorder_traversal(node->left);
    postorder_traversal(node->right);
    printf("%d ", node->data);
}

// Utility functions
int get_tree_height(TreeNode* node) {
    if (node == NULL) {
        return -1; // Height of an empty tree
    }
    int left_height = get_tree_height(node->left);
    int right_height = get_tree_height(node->right);
    return 1 + (left_height > right_height ? left_height : right_height);
}

TreeNode* find_min_node(TreeNode* node) {
    // Iterative version for public API as discussed
    if (node == NULL) {
        return NULL;
    }
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

TreeNode* find_max_node(TreeNode* node) {
    // Iterative version
    if (node == NULL) {
        return NULL;
    }
    while (node->right != NULL) {
        node = node->right;
    }
    return node;
}

size_t get_tree_size(BinaryTree* tree) {
    if (tree == NULL) {
        return 0;
    }
    return tree->size;
}
