#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../src/rbtree.h"

// Helper function prototypes for property checking
int check_rb_properties(RBTree* tree);
static int check_rb_properties_recursive(RBTree* tree, RBNode* node, int* black_height);
static int get_node_black_height(RBTree* tree, RBNode* node); // Helper for property 5

// Test function prototypes
void test_create_destroy_rbtree(void);
void test_rb_insert_simple(void);
void test_rb_insert_complex_rotations_recoloring(void);
void test_rb_delete_simple(void);
void test_rb_delete_complex_fixups(void);
void test_rb_search(void);
void test_rb_properties_after_ops(void);

// Main function to run all tests
int main() {
    printf("Running Red-Black Tree tests...\n\n");

    test_create_destroy_rbtree();
    test_rb_search(); // Test search early with empty/simple trees
    test_rb_insert_simple();
    test_rb_insert_complex_rotations_recoloring();
    test_rb_delete_simple();
    test_rb_delete_complex_fixups();
    test_rb_properties_after_ops();

    printf("\nAll Red-Black Tree tests completed!\n");
    return 0;
}

// --- Enhanced RB Property Checker ---

// Recursive helper to check properties 4 (Red node's children are black)
// and 5 (All paths from a node to its NIL descendants contain the same number of black nodes).
// Returns the black height of the valid subtree rooted at 'node'.
// Returns -1 if a property is violated.
static int check_rb_properties_recursive(RBTree* tree, RBNode* node, int* bh_check_violations) {
    if (node == tree->NIL) {
        return 1; // Black height of a NIL leaf path is 1 (for the NIL itself)
    }

    // Property 4: If a node is red, then both its children are black.
    if (node->color == RED) {
        if (node->left->color == RED || node->right->color == RED) {
            fprintf(stderr, "Property 4 Violation: Red node %d has a red child.\n", node->data);
            *bh_check_violations = 1;
            return -1; // Indicate violation
        }
    }

    // Recursively check left and right subtrees
    int left_black_height = check_rb_properties_recursive(tree, node->left, bh_check_violations);
    if (left_black_height == -1) return -1; // Propagate violation signal

    int right_black_height = check_rb_properties_recursive(tree, node->right, bh_check_violations);
    if (right_black_height == -1) return -1; // Propagate violation signal

    // Property 5: Black height consistency
    if (left_black_height != right_black_height) {
        fprintf(stderr, "Property 5 Violation: Black height mismatch at node %d (LBH=%d, RBH=%d).\n",
                node->data, left_black_height, right_black_height);
        *bh_check_violations = 1;
        return -1; // Indicate violation
    }

    // Return black height of this node's subtree
    return (node->color == BLACK ? 1 : 0) + left_black_height; // or right_black_height
}


int check_rb_properties(RBTree* tree) {
    if (tree == NULL) {
        fprintf(stderr, "Error: Tree is NULL.\n");
        return 1; // Invalid tree
    }
    if (tree->NIL == NULL) {
        fprintf(stderr, "Error: tree->NIL is NULL.\n");
        return 1;
    }
    if (tree->NIL->color != BLACK) {
        fprintf(stderr, "Property 3 Violation: tree->NIL is not BLACK.\n");
        return 3;
    }
    if (tree->root == tree->NIL && tree->size == 0) {
        return 0; // Empty tree is valid
    }
    if (tree->root == tree->NIL && tree->size != 0) {
        fprintf(stderr, "Error: Root is NIL but size is non-zero.\n");
        return 1;
    }


    // Property 2: The root is black.
    if (tree->root->color != BLACK) {
        fprintf(stderr, "Property 2 Violation: Root node %d is not BLACK.\n", tree->root->data);
        return 2;
    }
    
    int violations = 0;
    check_rb_properties_recursive(tree, tree->root, &violations);

    if (violations) return 45; // Indicates property 4 or 5 violation from recursive check
    return 0; // All properties hold
}


// --- Test Function Implementations ---

void test_create_destroy_rbtree(void) {
    printf("  Testing R-B tree creation and destruction...\n");
    RBTree *tree = create_rbtree();
    assert(tree != NULL);
    assert(tree->NIL != NULL);
    assert(tree->root == tree->NIL);
    assert(tree->NIL->color == BLACK);
    assert(tree->size == 0);
    assert(get_rbtree_size(tree) == 0);
    assert(check_rb_properties(tree) == 0); // Empty tree should be valid

    destroy_rbtree(tree);

    destroy_rbtree(NULL); // Should not crash
    printf("  test_create_destroy_rbtree: PASSED\n");
}

void test_rb_search(void) {
    printf("  Testing R-B tree search...\n");
    RBTree *tree = create_rbtree();
    assert(tree != NULL);

    assert(search_rbnode(tree, 100) == tree->NIL); // Search in empty tree

    // Insert some nodes (simple sequence, properties checked in insert tests)
    insert_rbnode(tree, 10);
    insert_rbnode(tree, 5);
    insert_rbnode(tree, 15);
    insert_rbnode(tree, 3);
    insert_rbnode(tree, 7);
    assert(tree->size == 5);


    assert(search_rbnode(tree, 10) != tree->NIL && search_rbnode(tree, 10)->data == 10);
    assert(search_rbnode(tree, 3) != tree->NIL && search_rbnode(tree, 3)->data == 3);
    assert(search_rbnode(tree, 7) != tree->NIL && search_rbnode(tree, 7)->data == 7);
    assert(search_rbnode(tree, 15) != tree->NIL && search_rbnode(tree, 15)->data == 15);
    assert(search_rbnode(tree, 5) != tree->NIL && search_rbnode(tree, 5)->data == 5);

    assert(search_rbnode(tree, 100) == tree->NIL); // Search for non-existent
    assert(search_rbnode(tree, 1) == tree->NIL);
    assert(search_rbnode(tree, 12) == tree->NIL);

    destroy_rbtree(tree);
    printf("  test_rb_search: PASSED\n");
}


void test_rb_insert_simple(void) {
    printf("  Testing R-B tree simple insertions with property checks...\n");
    RBTree *tree = create_rbtree();
    assert(tree != NULL);

    // Insert 10 (root)
    assert(insert_rbnode(tree, 10) == 0); // Success
    assert(tree->size == 1 && get_rbtree_size(tree) == 1);
    assert(tree->root->data == 10);
    assert(check_rb_properties(tree) == 0);

    // Insert 5 (left child of 10, becomes RED)
    assert(insert_rbnode(tree, 5) == 0);
    assert(tree->size == 2 && get_rbtree_size(tree) == 2);
    assert(search_rbnode(tree, 5)->color == RED);
    assert(check_rb_properties(tree) == 0);

    // Insert 15 (right child of 10, becomes RED)
    // Structure: 10(B), 5(R), 15(R)
    assert(insert_rbnode(tree, 15) == 0);
    assert(tree->size == 3 && get_rbtree_size(tree) == 3);
    assert(search_rbnode(tree, 15)->color == RED);
    assert(check_rb_properties(tree) == 0);

    // Insert 3 (child of 5, triggers fixup: 5(B), 3(R), 10(B), 15(R) -> after rotation 5(B) root of this subtree)
    // Expected: 10(B) remains root, 5(B) left, 15(B) right, 3(R) child of 5.
    // Actual after CLRS insert_fixup: 10(B) is root. 5(L) is RED. Insert 3. z=3(R), p=5(R), pp=10(B). Uncle 15(R). Case 1.
    // p=5 becomes BLACK, uncle=15 becomes BLACK. pp=10 becomes RED. z becomes 10.
    // Loop continues: z=10(R), p=NIL(B). Loop terminates. Root 10 becomes BLACK.
    // So: 10(B), 5(B), 15(B), 3(R) under 5.
    assert(insert_rbnode(tree, 3) == 0);
    assert(tree->size == 4 && get_rbtree_size(tree) == 4);
    assert(check_rb_properties(tree) == 0);
    // print_rbtree_structure(tree, tree->root, 0);


    // Insert 7 (child of 5, triggers fixup)
    // Current: 10(B), 5(B), 3(R), 15(B). Insert 7. z=7(R), p=5(B). No fixup needed initially for z->parent color.
    // Ah, 5 was B. So 7(R) under 5(B) is fine.
    assert(insert_rbnode(tree, 7) == 0);
    assert(tree->size == 5 && get_rbtree_size(tree) == 5);
    assert(check_rb_properties(tree) == 0);
    // print_rbtree_structure(tree, tree->root, 0);

    // Insert 1 (triggers rotation at 5, then possibly up)
    // Current: 10(B), [ L: 5(B), [L:3(R)], [R:7(R)] ], [ R: 15(B) ]
    // Insert 1. z=1(R), p=3(R), pp=5(B). Uncle of 3 is 7(R). Case 1.
    // p=3 becomes BLACK. uncle=7 becomes BLACK. pp=5 becomes RED. z becomes 5.
    // Loop continues: z=5(R), p=10(B). Loop terminates. Root 10 is BLACK.
    // So: 10(B), [L: 5(R), [L:3(B)], [R:7(B)] ], [R: 15(B)]. This is a Red-Red violation (5 and its parent 10).
    // This means my manual trace of insert 3 was slightly off in its resulting colors for 5 and 15.
    // Let's re-trace insert 3: 10(B), 5(R), 15(R). Insert 3. z=3(R), p=5(R), pp=10(B), uncle y=15(R).
    // Case 1: p->color=BLACK (5->B), y->color=BLACK (15->B), pp->color=RED (10->R). z = pp (10).
    // Loop again: z=10(R), z->parent is NIL (BLACK). Loop terminates. tree->root->color = BLACK. (10->B)
    // So after 3: 10(B), 5(B), 15(B), 3(R) child of 5. Correct.
    // Now insert 1: z=1(R), p=3(R), pp=5(B). Uncle of p (node 7) is RED.
    // Case 1: p->color=BLACK (3->B), uncle->color=BLACK (7->B), pp->color=RED (5->R). z=pp (5).
    // Loop again: z=5(R), p=10(B). Loop terminates. tree->root->color = BLACK. (10->B)
    // So after 1: 10(B), [L: 5(R), [L:3(B), [L:1(R)]], [R:7(B)] ], [R: 15(B)].
    // This tree structure is valid.
    assert(insert_rbnode(tree, 1) == 0);
    assert(tree->size == 6 && get_rbtree_size(tree) == 6);
    assert(check_rb_properties(tree) == 0);
    // print_rbtree_structure(tree, tree->root, 0);

    // Test inserting duplicate
    assert(insert_rbnode(tree, 10) == 1); // 1 for exists
    assert(tree->size == 6);
    assert(check_rb_properties(tree) == 0);

    destroy_rbtree(tree);
    printf("  test_rb_insert_simple: PASSED\n");
}

void test_rb_insert_complex_rotations_recoloring(void) {
    printf("  Testing R-B tree complex insertions (rotations, recoloring)...\n");
    RBTree *tree;

    // Test Case: Ascending order (triggers left rotations and recoloring)
    tree = create_rbtree();
    int asc_values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (int i = 0; i < sizeof(asc_values)/sizeof(asc_values[0]); ++i) {
        assert(insert_rbnode(tree, asc_values[i]) == 0);
        assert(tree->size == (size_t)i + 1);
        assert(check_rb_properties(tree) == 0);
    }
    // printf("Ascending Order Tree:\n"); print_rbtree_structure(tree, tree->root, 0);
    destroy_rbtree(tree);

    // Test Case: Descending order (triggers right rotations and recoloring)
    tree = create_rbtree();
    int desc_values[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    for (int i = 0; i < sizeof(desc_values)/sizeof(desc_values[0]); ++i) {
        assert(insert_rbnode(tree, desc_values[i]) == 0);
        assert(tree->size == (size_t)i + 1);
        assert(check_rb_properties(tree) == 0);
    }
    // printf("Descending Order Tree:\n"); print_rbtree_structure(tree, tree->root, 0);
    destroy_rbtree(tree);

    // Test Case: Specific sequence for Case 1, 2, 3 (Left-Right, Left-Left, Right-Left, Right-Right)
    // Example sequence from a common textbook/visualization: 10, 85, 15, 70, 20, 60, 30, 50, 65, 80, 90, 40, 5, 55
    tree = create_rbtree();
    int clrs_values[] = {10, 85, 15, 70, 20, 60, 30, 50, 65, 80, 90, 40, 5, 55}; // A more complex sequence
    for (int i = 0; i < sizeof(clrs_values)/sizeof(clrs_values[0]); ++i) {
        // printf("Inserting %d\n", clrs_values[i]);
        assert(insert_rbnode(tree, clrs_values[i]) == 0);
        assert(tree->size == (size_t)i + 1);
        assert(check_rb_properties(tree) == 0);
        // print_rbtree_structure(tree, tree->root, 0);
    }
    // printf("CLRS Sequence Tree:\n"); print_rbtree_structure(tree, tree->root, 0);
    destroy_rbtree(tree);
    
    printf("  test_rb_insert_complex_rotations_recoloring: PASSED\n");
}

void test_rb_delete_simple(void) {
    printf("  Testing R-B tree simple deletions...\n");
    RBTree *tree = create_rbtree();

    // Insert a few nodes
    insert_rbnode(tree, 10);
    insert_rbnode(tree, 5);
    insert_rbnode(tree, 15);
    insert_rbnode(tree, 3);  // Leaf, RED (after 10,5,15,3 -> 10B, 5B, 15B, 3R)
    insert_rbnode(tree, 7);  // Leaf, RED (10B, 5B,3R,7R, 15B)
    assert(tree->size == 5);
    assert(check_rb_properties(tree) == 0);
    // print_rbtree_structure(tree, tree->root, 0); // Initial: 10B, 5B(3R,7R), 15B

    // Delete a RED leaf node (e.g., 3)
    // printf("Deleting 3 (RED leaf)\n");
    assert(delete_rbnode(tree, 3) == 0); // Success
    assert(tree->size == 4);
    assert(search_rbnode(tree, 3) == tree->NIL);
    assert(check_rb_properties(tree) == 0);
    // print_rbtree_structure(tree, tree->root, 0); // Expected: 10B, 5B(7R), 15B

    // Delete another RED leaf node (e.g., 7)
    // printf("Deleting 7 (RED leaf)\n");
    assert(delete_rbnode(tree, 7) == 0);
    assert(tree->size == 3);
    assert(search_rbnode(tree, 7) == tree->NIL);
    assert(check_rb_properties(tree) == 0);
    // print_rbtree_structure(tree, tree->root, 0); // Expected: 10B, 5B, 15B

    // Delete a BLACK leaf node (e.g., 15, requires fixup)
    // printf("Deleting 15 (BLACK leaf)\n");
    assert(delete_rbnode(tree, 15) == 0);
    assert(tree->size == 2);
    assert(search_rbnode(tree, 15) == tree->NIL);
    assert(check_rb_properties(tree) == 0);
    // print_rbtree_structure(tree, tree->root, 0); // Expected: 5B is root, 10R is child, or 10B root, 5R child

    // Delete root (e.g., 10, if 10 is root, 5 is its child)
    // Current tree structure might be 5(B) <- 10(R) or 10(B) -> 5(R)
    // If 5(B) is root, 10(R) its child. Delete 10(R child). Simple.
    // If 10(B) is root, 5(R) its child. Delete 10(B root). 5 becomes new root, colored BLACK.
    // printf("Deleting 10\n");
    assert(delete_rbnode(tree, 10) == 0);
    assert(tree->size == 1);
    assert(search_rbnode(tree, 10) == tree->NIL);
    assert(check_rb_properties(tree) == 0);
    // print_rbtree_structure(tree, tree->root, 0); // Expected: 5B (root)

    // Delete the last node (e.g., 5)
    // printf("Deleting 5 (last node)\n");
    assert(delete_rbnode(tree, 5) == 0);
    assert(tree->size == 0);
    assert(search_rbnode(tree, 5) == tree->NIL);
    assert(tree->root == tree->NIL);
    assert(check_rb_properties(tree) == 0); // Empty tree

    // Test deleting from empty tree
    assert(delete_rbnode(tree, 100) == 1); // Not found
    assert(tree->size == 0);

    destroy_rbtree(tree);
    printf("  test_rb_delete_simple: PASSED\n");
}

void test_rb_delete_complex_fixups(void) {
    printf("  Testing R-B tree complex deletions with fixups...\n");
    RBTree *tree;
    int values[] = {10, 5, 15, 3, 7, 12, 17, 1, 4, 6, 8, 11, 13, 16, 18}; // A balanced set of 15 nodes
    
    // Scenario 1: Trigger various delete cases
    tree = create_rbtree();
    for(int i=0; i < sizeof(values)/sizeof(values[0]); ++i) {
        insert_rbnode(tree, values[i]);
    }
    assert(tree->size == 15);
    assert(check_rb_properties(tree) == 0);
    // printf("Initial tree for complex delete test:\n"); print_rbtree_structure(tree, tree->root, 0);

    // Deletion sequence designed to hit different fixup cases
    // This is hard to design without a visualizer and step-by-step state check.
    // We rely heavily on check_rb_properties.
    int delete_seq[] = {1, 4, 6, 8, 11, 13, 16, 18, 3, 7, 12, 17, 5, 15, 10}; // Delete all, leaves first, then up
    
    for(int i=0; i < sizeof(delete_seq)/sizeof(delete_seq[0]); ++i) {
        // printf("Deleting %d\n", delete_seq[i]);
        assert(delete_rbnode(tree, delete_seq[i]) == 0);
        assert(tree->size == (sizeof(values)/sizeof(values[0])) - (size_t)(i+1));
        assert(check_rb_properties(tree) == 0);
        // print_rbtree_structure(tree, tree->root, 0);
    }
    assert(tree->root == tree->NIL);
    destroy_rbtree(tree);

    // Add more specific scenarios if certain fixup cases are hard to hit with generic sequences
    // For example, delete a node whose sibling is RED, and that sibling's children are BLACK.
    // Case D1: x's sibling w is red.
    tree = create_rbtree();
    // Build: 10(B), 5(B), 20(R,-,30(B)), 25(R) -> ... 20(B), 10(R), 30(R), 25(B)
    insert_rbnode(tree, 10); insert_rbnode(tree, 5); insert_rbnode(tree, 20); insert_rbnode(tree, 25); insert_rbnode(tree, 30);
    // Make 20 RED, 10 BLACK, 5 BLACK, 25 BLACK, 30 RED. (Manually for testing specific case if needed)
    // Or just use a sequence that creates this: 10, 5, 30, 20, 25.
    // After 10,5,30,20,25: Root 10(B), 5(B) L, 25(B) R. 20(R) L of 25. 30(R) R of 25.
    // This is valid: 10(B), 5(B), 25(B, L:20(R), R:30(R)).
    // Delete 5 (BLACK node, x is NIL, x->parent is 10). x's sibling w is 25(B).
    // This is not Case D1.
    // To get Case D1: sibling w is RED.
    // Need a structure like: P(B), X(B), W(R).  Delete X.
    // Example: Insert 10, 5, 20. Then delete 5. Root is 10(B), 5(R), 20(R). This is invalid.
    // After 10,5,20: 10(B), 5(R), 20(R).
    // Insert 1: 10(B), 5(B), 1(R), 20(B).
    // Insert 25: 10(B), 5(B),1(R), 20(B),25(R).
    // Delete 1. (RED leaf) -> 10(B), 5(B), 20(B), 25(R).
    // Delete 5. x is NIL, parent is 10. Sibling w is 20(B). Not D1.
    // This shows that crafting specific scenarios for each delete case is complex.
    // For now, the general sequence and property check is the main test.
    destroy_rbtree(tree);


    printf("  test_rb_delete_complex_fixups: PASSED (relies on check_rb_properties)\n");
}


void test_rb_properties_after_ops(void) {
    printf("  Testing R-B tree properties after a mixed sequence of operations (Stress Test)...\n");
    RBTree *tree = create_rbtree();
    const int num_ops = 200;
    const int max_val = 1000;

    for (int i = 0; i < num_ops; ++i) {
        int op_type = rand() % 2; // 0 for insert, 1 for delete
        int value = rand() % max_val;

        if (op_type == 0 || tree->size < 5) { // Insert or if tree is too small to delete meaningfully
            // printf("Stress Test: Inserting %d\n", value);
            insert_rbnode(tree, value); // Return value (0,1,-1) can be checked but not critical for this test
        } else {
            // printf("Stress Test: Deleting %d\n", value);
            delete_rbnode(tree, value); // Return value (0,1,-1) can be checked
        }
        
        // Check properties after every few operations or every op if fast enough
        if (i % 10 == 0 || i == num_ops -1) {
             // printf("  Stress Test: Current Size: %zu\n", tree->size);
             // if (tree->size > 0 && tree->root != tree->NIL) print_rbtree_structure(tree, tree->root, 0);
            assert(check_rb_properties(tree) == 0);
        }
    }
    // Final check
    assert(check_rb_properties(tree) == 0);
    // printf("Final tree size: %zu\n", tree->size);
    // if (tree->size > 0) print_rbtree_structure(tree, tree->root, 0);

    destroy_rbtree(tree);
    printf("  test_rb_properties_after_ops: PASSED\n");
}
