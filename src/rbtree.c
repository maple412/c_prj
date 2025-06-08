#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h> // For malloc, free

// Static helper function prototypes
static RBNode* create_rb_node_internal(RBTree* tree, int data); // Renamed to avoid conflict if a public create_rb_node was intended
static void destroy_rbnodes_recursive(RBTree* tree, RBNode* node);
static void left_rotate(RBTree* tree, RBNode* x);
static void right_rotate(RBTree* tree, RBNode* y);
static void rb_transplant(RBTree* tree, RBNode* u, RBNode* v);
static void rb_insert_fixup(RBTree* tree, RBNode* z);
static RBNode* rbtree_minimum_node(RBTree* tree, RBNode* node);
static void rb_delete_fixup(RBTree* tree, RBNode* x);


// Lifecycle
RBTree* create_rbtree(void) {
    RBTree* tree = (RBTree*)malloc(sizeof(RBTree));
    if (tree == NULL) {
        perror("Failed to allocate memory for RBTree");
        return NULL;
    }

    tree->NIL = (RBNode*)malloc(sizeof(RBNode));
    if (tree->NIL == NULL) {
        perror("Failed to allocate memory for NIL node");
        free(tree);
        return NULL;
    }
    tree->NIL->color = BLACK;
    tree->NIL->data = 0; 
    tree->NIL->parent = tree->NIL; 
    tree->NIL->left = tree->NIL;   
    tree->NIL->right = tree->NIL;  

    tree->root = tree->NIL; 
    tree->size = 0;

    return tree;
}

static void destroy_rbnodes_recursive(RBTree* tree, RBNode* node) {
    if (node == tree->NIL) {
        return;
    }
    // Ensure we don't try to free NIL if it was mistakenly passed and not tree->NIL
    if (node == NULL) return; 

    destroy_rbnodes_recursive(tree, node->left);
    destroy_rbnodes_recursive(tree, node->right);
    free(node);
}

void destroy_rbtree(RBTree* tree) {
    if (tree == NULL) {
        return;
    }
    destroy_rbnodes_recursive(tree, tree->root);
    
    if (tree->NIL != NULL) {
        free(tree->NIL);
        tree->NIL = NULL;
    }
    free(tree);
}

// Helper to create a new RBNode
static RBNode* create_rb_node_internal(RBTree* tree, int data) {
    RBNode* newNode = (RBNode*)malloc(sizeof(RBNode));
    if (newNode == NULL) {
        perror("Failed to allocate memory for RBNode");
        return NULL; 
    }
    newNode->data = data;
    // New nodes are typically RED, parent and children are set by caller (BST insert logic)
    // Color is set to RED in insert_rbnode explicitly.
    // Children must be tree->NIL.
    newNode->color = RED; // Default color for new node before fixup
    newNode->parent = tree->NIL; // Will be set properly by insert logic
    newNode->left = tree->NIL;
    newNode->right = tree->NIL;
    return newNode;
}


// Rotations
static void left_rotate(RBTree* tree, RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left != tree->NIL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->NIL) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

static void right_rotate(RBTree* tree, RBNode* y) {
    RBNode* x = y->left;
    y->left = x->right;
    if (x->right != tree->NIL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == tree->NIL) {
        tree->root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

// Transplant
static void rb_transplant(RBTree* tree, RBNode* u, RBNode* v) {
    if (u->parent == tree->NIL) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent; // v can be tree->NIL, its parent will be updated.
                           // tree->NIL->parent should remain tree->NIL ideally,
                           // but CLRS updates v->parent regardless. This is fine as NIL's parent is not used
                           // in a way that would break logic if temporarily changed.
}

// Insert Fixup
static void rb_insert_fixup(RBTree* tree, RBNode* z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) { // Parent is left child
            RBNode* y = z->parent->parent->right; // Uncle
            if (y->color == RED) { // Case 1: Uncle is RED
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else { // Uncle is BLACK
                if (z == z->parent->right) { // Case 2: z is right child
                    z = z->parent;
                    left_rotate(tree, z);
                }
                // Case 3: z is left child (or became parent in Case 2)
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(tree, z->parent->parent);
            }
        } else { // Parent is right child (symmetric)
            RBNode* y = z->parent->parent->left; // Uncle
            if (y->color == RED) { // Case 1
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else { // Uncle is BLACK
                if (z == z->parent->left) { // Case 2
                    z = z->parent;
                    right_rotate(tree, z);
                }
                // Case 3
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

// Insert
int insert_rbnode(RBTree* tree, int data) {
    if (tree == NULL) {
        return -1; // Or some error code for NULL tree
    }

    RBNode* z = create_rb_node_internal(tree, data);
    if (z == NULL) {
        return -1; // Malloc failure
    }

    RBNode* y = tree->NIL;
    RBNode* x = tree->root;

    while (x != tree->NIL) {
        y = x;
        if (z->data < x->data) {
            x = x->left;
        } else if (z->data > x->data) {
            x = x->right;
        } else { // Duplicate data
            free(z); // Free the newly created node
            return 1; // Indicate duplicate
        }
    }

    z->parent = y;
    if (y == tree->NIL) { // Tree was empty
        tree->root = z;
    } else if (z->data < y->data) {
        y->left = z;
    } else {
        y->right = z;
    }

    // z->left and z->right are already tree->NIL from create_rb_node_internal
    // z->color is already RED from create_rb_node_internal

    rb_insert_fixup(tree, z);
    tree->size++;
    return 0; // Success
}


// Minimum node in subtree
static RBNode* rbtree_minimum_node(RBTree* tree, RBNode* node) {
    if (node == NULL || node == tree->NIL) return tree->NIL; 
    while (node->left != tree->NIL) {
        node = node->left;
    }
    return node;
}

// Delete Fixup
static void rb_delete_fixup(RBTree* tree, RBNode* x) {
    while (x != tree->root && x->color == BLACK) {
        if (x == x->parent->left) {
            RBNode* w = x->parent->right; // Sibling
            if (w->color == RED) { // Case 1: Sibling w is red
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate(tree, x->parent);
                w = x->parent->right;
            }
            // Case 2: Sibling w is black, and both of w's children are black
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                // Case 3: Sibling w is black, w's left child is red, and w's right child is black
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(tree, w);
                    w = x->parent->right;
                }
                // Case 4: Sibling w is black, and w's right child is red
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate(tree, x->parent);
                x = tree->root; // Exit loop
            }
        } else { // Symmetric: x is right child
            RBNode* w = x->parent->left; // Sibling
            if (w->color == RED) { // Case 1
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate(tree, x->parent);
                w = x->parent->left;
            }
            // Case 2
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                // Case 3
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(tree, w);
                    w = x->parent->left;
                }
                // Case 4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate(tree, x->parent);
                x = tree->root; // Exit loop
            }
        }
    }
    x->color = BLACK;
}


// Delete
int delete_rbnode(RBTree* tree, int data) {
    if (tree == NULL) {
        return -1; // Error: NULL tree
    }
    RBNode* z = search_rbnode(tree, data);
    if (z == tree->NIL) {
        return 1; // Node not found
    }

    RBNode* y = z;
    RBNode* x; // Node that moves into y's original position, or y's child that might need fixup
    Color original_y_color = y->color;

    if (z->left == tree->NIL) {
        x = z->right;
        rb_transplant(tree, z, z->right);
    } else if (z->right == tree->NIL) {
        x = z->left;
        rb_transplant(tree, z, z->left);
    } else {
        y = rbtree_minimum_node(tree, z->right); // Successor
        original_y_color = y->color;
        x = y->right; // x could be tree->NIL

        if (y->parent == z) {
            // This condition is important for x->parent if x is NIL.
            // If x is tree->NIL, its parent is already tree->NIL (or itself),
            // but for fixup logic, it needs to point to where y *was*.
            x->parent = y; // Set parent of x (even if NIL) to y (successor's original position)
        } else {
            rb_transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rb_transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color; // y takes on z's original color
    }

    free(z);
    tree->size--;

    if (original_y_color == BLACK) {
        rb_delete_fixup(tree, x);
    }
    return 0; // Success
}


// Search
RBNode* search_rbnode(RBTree* tree, int data) {
    if (tree == NULL) return NULL; 
    RBNode* current = tree->root;
    while (current != tree->NIL) {
        if (data == current->data) {
            return current;
        } else if (data < current->data) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return tree->NIL; // Not found
}

// Traversals/Utilities
void inorder_rbtree_traversal(RBTree* tree, RBNode* node) {
    if (node != tree->NIL && node != NULL) { // Added node != NULL for safety, though NIL should be main check
        inorder_rbtree_traversal(tree, node->left);
        printf("%d(%c) ", node->data, node->color == RED ? 'R' : 'B');
        inorder_rbtree_traversal(tree, node->right);
    }
}

void print_rbtree_structure_recursive(RBTree* tree, RBNode* node, int level, int is_left) {
    if (node == tree->NIL || node == NULL) {
        return;
    }
    for (int i = 0; i < level -1; i++) {
        printf(i == level - 2 ? "|----" : "     ");
    }
     if (level > 0) {
        printf("%s: ", is_left ? "L" : "R");
    }

    printf("%d(%c)\n", node->data, node->color == RED ? 'R' : 'B');

    print_rbtree_structure_recursive(tree, node->left, level + 1, 1);
    print_rbtree_structure_recursive(tree, node->right, level + 1, 0);
}


void print_rbtree_structure(RBTree* tree, RBNode* node, int level) {
    if (tree == NULL || tree->root == tree->NIL) {
        printf("Tree is empty or NULL.\n");
        return;
    }
    printf("RBTree Structure (Size: %zu):\n", tree->size);
    print_rbtree_structure_recursive(tree, tree->root, 0, 0); // Start with root
}


size_t get_rbtree_size(RBTree* tree) {
    if (tree == NULL) {
        return 0;
    }
    return tree->size;
}
