#include "avl_tree.h"
#include <stdio.h>
#include <stdlib.h> // For malloc, free, size_t
#include <stddef.h> // For NULL

// --- Static Helper Function Prototypes ---

static AVLNode* create_new_avl_node(int key);
static void destroy_avl_nodes_recursive(AVLNode* node);
static int get_node_height_internal(AVLNode* node);
static int get_balance_factor_internal(AVLNode* node);
static void update_node_height_internal(AVLNode* node);
static int max_int_internal(int a, int b);
static AVLNode* right_rotate_internal(AVLNode* y);
static AVLNode* left_rotate_internal(AVLNode* x);
static AVLNode* insert_recursive_internal(AVLNode* node, int key, int* success_flag, size_t* size_ptr);
static AVLNode* delete_recursive_internal(AVLNode* node, int key, int* success_flag, size_t* size_ptr);
static AVLNode* find_min_node_internal(AVLNode* node);


// --- Static Helper Function Implementations ---

static AVLNode* create_new_avl_node(int key) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (node == NULL) {
        perror("Failed to allocate memory for AVLNode");
        return NULL;
    }
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 0; // Height of a new node (leaf) is 0
    return node;
}

static int max_int_internal(int a, int b) {
    return (a > b) ? a : b;
}

static int get_node_height_internal(AVLNode* node) {
    if (node == NULL) {
        return -1; // Height of NULL node for balance factor calculation
    }
    return node->height;
}

static void update_node_height_internal(AVLNode* node) {
    if (node != NULL) {
        node->height = 1 + max_int_internal(get_node_height_internal(node->left), get_node_height_internal(node->right));
    }
}

static int get_balance_factor_internal(AVLNode* node) {
    if (node == NULL) {
        return 0;
    }
    return get_node_height_internal(node->left) - get_node_height_internal(node->right);
}

static AVLNode* right_rotate_internal(AVLNode* y) {
    if (y == NULL || y->left == NULL) return y; // Cannot perform right rotation
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    update_node_height_internal(y);
    update_node_height_internal(x);

    return x; // New root of this rotated subtree
}

static AVLNode* left_rotate_internal(AVLNode* x) {
    if (x == NULL || x->right == NULL) return x; // Cannot perform left rotation
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    update_node_height_internal(x);
    update_node_height_internal(y);

    return y; // New root of this rotated subtree
}

static AVLNode* find_min_node_internal(AVLNode* node) {
    AVLNode* current = node;
    while (current != NULL && current->left != NULL) {
        current = current->left;
    }
    return current;
}


static AVLNode* insert_recursive_internal(AVLNode* node, int key, int* success_flag, size_t* size_ptr) {
    // 1. Standard BST insert
    if (node == NULL) {
        AVLNode* new_node = create_new_avl_node(key);
        if (new_node == NULL) {
            *success_flag = -1; // Malloc error
            return NULL;
        }
        (*size_ptr)++;
        *success_flag = 0; // Successfully inserted
        return new_node;
    }

    if (key < node->key) {
        node->left = insert_recursive_internal(node->left, key, success_flag, size_ptr);
    } else if (key > node->key) {
        node->right = insert_recursive_internal(node->right, key, success_flag, size_ptr);
    } else { // key == node->key
        *success_flag = 1; // Key already exists
        return node;
    }
    
    // If malloc failed in recursive call, propagate NULL up
    if (*success_flag == -1 && ( (key < node->key && node->left == NULL) || (key > node->key && node->right == NULL) )) {
        // This check is tricky. If a child became NULL due to malloc error deeper, this node itself isn't NULL yet.
        // The actual check for NULL child due to malloc failure is implicitly handled: if node->left/right is NULL,
        // height calculation and balancing will still proceed, but the tree structure is already compromised.
        // The most important is that the final tree->root is NULL if the very first node allocation failed.
        // The success_flag = -1 is the primary indicator of such failure.
    }


    // 2. Update height of current node
    update_node_height_internal(node);

    // 3. Get balance factor
    int balance = get_balance_factor_internal(node);

    // 4. Perform rotations if unbalanced (LL, LR, RR, RL cases)
    // Left Left Case
    if (balance > 1 && key < node->left->key) {
        return right_rotate_internal(node);
    }
    // Right Right Case
    if (balance < -1 && key > node->right->key) {
        return left_rotate_internal(node);
    }
    // Left Right Case
    if (balance > 1 && key > node->left->key) {
        node->left = left_rotate_internal(node->left);
        return right_rotate_internal(node);
    }
    // Right Left Case
    if (balance < -1 && key < node->right->key) {
        node->right = right_rotate_internal(node->right);
        return left_rotate_internal(node);
    }

    return node; // Return the (possibly new) root of this subtree
}

static AVLNode* delete_recursive_internal(AVLNode* node, int key, int* success_flag, size_t* size_ptr) {
    if (node == NULL) {
        *success_flag = 1; // Key not found
        return NULL;
    }

    // Standard BST delete
    if (key < node->key) {
        node->left = delete_recursive_internal(node->left, key, success_flag, size_ptr);
    } else if (key > node->key) {
        node->right = delete_recursive_internal(node->right, key, success_flag, size_ptr);
    } else { // key == node->key, node to be deleted found
        *success_flag = 0; // Mark as potentially successful deletion

        if (node->left == NULL || node->right == NULL) { // Node with one or no child
            AVLNode* temp = node->left ? node->left : node->right;
            if (temp == NULL) { // No child case
                temp = node; // Store node to be freed
                node = NULL; // Make current node NULL
            } else { // One child case
                // Copy contents of the non-empty child to this node
                // This is effectively like bypassing the original node.
                // The child 'temp' will be freed.
                // Or, more standard: make parent point to 'temp', then free 'node'.
                // With recursive approach, we return 'temp' to be linked by parent.
                AVLNode* node_to_free = node;
                node = temp; // The child takes the place of the node.
                temp = node_to_free; // 'temp' now points to original node to be freed
            }
            free(temp);
            (*size_ptr)--;
        } else { // Node with two children
            AVLNode* temp = find_min_node_internal(node->right); // Find inorder successor
            node->key = temp->key; // Copy successor's key to this node
            // Delete the inorder successor from the right subtree
            // Success flag from this recursive call is important to ensure it was actually found and deleted.
            // For this specific recursive call, we expect it to succeed.
            int inner_success_flag = 1; 
            node->right = delete_recursive_internal(node->right, temp->key, &inner_success_flag, size_ptr);
            if (inner_success_flag != 0) {
                // This would be an internal error, successor should always be found.
                // Could set main *success_flag to an error code here.
                // For now, assume successor deletion is successful.
            }
        }
    }

    if (node == NULL) { // If tree became empty or node was deleted
        return NULL;
    }

    // Update height of current node
    update_node_height_internal(node);

    // Get balance factor
    int balance = get_balance_factor_internal(node);

    // Perform rotations if unbalanced
    // Left Left Case (balance > 1, left child's balance >= 0)
    if (balance > 1 && get_balance_factor_internal(node->left) >= 0) {
        return right_rotate_internal(node);
    }
    // Left Right Case (balance > 1, left child's balance < 0)
    if (balance > 1 && get_balance_factor_internal(node->left) < 0) {
        node->left = left_rotate_internal(node->left);
        return right_rotate_internal(node);
    }
    // Right Right Case (balance < -1, right child's balance <= 0)
    if (balance < -1 && get_balance_factor_internal(node->right) <= 0) {
        return left_rotate_internal(node);
    }
    // Right Left Case (balance < -1, right child's balance > 0)
    if (balance < -1 && get_balance_factor_internal(node->right) > 0) {
        node->right = right_rotate_internal(node->right);
        return left_rotate_internal(node);
    }

    return node;
}


// --- Lifecycle Functions ---

AVLTree* create_avl_tree(void) {
    AVLTree* tree = (AVLTree*)malloc(sizeof(AVLTree));
    if (tree == NULL) {
        perror("Failed to allocate memory for AVLTree");
        return NULL;
    }
    tree->root = NULL;
    tree->size = 0;
    return tree;
}

static void destroy_avl_nodes_recursive(AVLNode* node) {
    if (node != NULL) {
        destroy_avl_nodes_recursive(node->left);
        destroy_avl_nodes_recursive(node->right);
        free(node);
    }
}

void destroy_avl_tree(AVLTree* tree) {
    if (tree == NULL) {
        return;
    }
    destroy_avl_nodes_recursive(tree->root);
    free(tree);
}

// --- Core Operations ---

int insert_into_avl_tree(AVLTree* tree, int key) {
    if (tree == NULL) {
        return -1; // Error: NULL tree
    }
    int success_flag = 0; // 0: inserted, 1: key exists, -1: malloc error
    
    tree->root = insert_recursive_internal(tree->root, key, &success_flag, &(tree->size));
    
    // Check if the root became NULL due to malloc error for the very first node
    if (tree->root == NULL && success_flag == -1 && tree->size == 0) {
        // Malloc error for the root node itself. Size would not have been incremented.
        // No specific action needed here other than returning the success_flag.
    }
    return success_flag;
}

int delete_from_avl_tree(AVLTree* tree, int key) {
    if (tree == NULL) { // Allow deletion from tree with NULL root (empty tree)
        return 1; // Key not found
    }
    if (tree->root == NULL && key) { // Tree is empty, key cannot be found
        return 1; 
    }

    int success_flag = 1; // 1 for not found initially
    
    tree->root = delete_recursive_internal(tree->root, key, &success_flag, &(tree->size));
    
    return success_flag; // 0 if deleted, 1 if not found
}

AVLNode* search_in_avl_tree(AVLTree* tree, int key) {
    if (tree == NULL) {
        return NULL;
    }
    AVLNode* current = tree->root;
    while (current != NULL) {
        if (key == current->key) {
            return current;
        } else if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return NULL; // Key not found
}


// --- Traversals & Utilities ---

void inorder_avl_traversal(AVLNode* node) {
    if (node != NULL) {
        inorder_avl_traversal(node->left);
        // printf("%d ", node->key); // Simple print
        printf("%d(H:%d,B:%d) ", node->key, get_node_height_internal(node), get_balance_factor_internal(node));
        inorder_avl_traversal(node->right);
    }
}

void preorder_avl_traversal(AVLNode* node) {
    if (node != NULL) {
        // printf("%d ", node->key);
        printf("%d(H:%d,B:%d) ", node->key, get_node_height_internal(node), get_balance_factor_internal(node));
        preorder_avl_traversal(node->left);
        preorder_avl_traversal(node->right);
    }
}

void postorder_avl_traversal(AVLNode* node) {
    if (node != NULL) {
        postorder_avl_traversal(node->left);
        postorder_avl_traversal(node->right);
        // printf("%d ", node->key);
        printf("%d(H:%d,B:%d) ", node->key, get_node_height_internal(node), get_balance_factor_internal(node));
    }
}

size_t get_avl_tree_size(AVLTree* tree) {
    if (tree == NULL) {
        return 0;
    }
    return tree->size;
}

int get_avl_tree_height(AVLTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        return -1; // Height of an empty tree
    }
    return tree->root->height; // Root's height is tree's height (as stored in node)
}
