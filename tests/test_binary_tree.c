#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h> // For potential future use with stdout capture

#include "../src/binary_tree.h"

// --- Test Helper: Array for Inorder Traversal Verification ---
#define MAX_TRAVERSAL_NODES 100
int traversal_output[MAX_TRAVERSAL_NODES];
int traversal_idx;

void record_inorder_value(int data) {
    if (traversal_idx < MAX_TRAVERSAL_NODES) {
        traversal_output[traversal_idx++] = data;
    }
}

// Modified inorder traversal to use callback (if we were allowed to change src)
// For now, we can't use this directly with the current src/binary_tree.c
/*
void inorder_traversal_with_callback(TreeNode* node, void (*callback)(int)) {
    if (node == NULL) return;
    inorder_traversal_with_callback(node->left, callback);
    if (callback) callback(node->data);
    inorder_traversal_with_callback(node->right, callback);
}
*/

// --- Test function prototypes ---
void test_create_destroy_tree(void);
void test_bst_insert_and_search(void);
void test_bst_delete(void);
void test_traversals(void);
void test_get_height(void);
void test_find_min_max(void);
void test_get_size(void);

// --- Main function to run all tests ---
int main() {
    printf("Running binary tree tests...\n\n");

    test_create_destroy_tree();
    test_get_size(); // Test size early with empty tree
    test_get_height(); // Test height early with empty tree
    test_find_min_max(); // Test min/max early with empty tree
    test_bst_insert_and_search();
    test_traversals(); // Test traversals after some insertions
    test_bst_delete(); // Test delete after insertions and other checks
    
    // Re-test utilities after deletions
    printf("\n  Re-testing utilities after deletions:\n");
    test_get_size(); 
    test_get_height();
    test_find_min_max();

    printf("\nBinary tree tests completed successfully!\n");
    return 0;
}

// --- Implementations of test functions ---

void test_create_destroy_tree(void) {
    printf("  Testing tree creation and destruction...\n");
    BinaryTree *tree = create_binary_tree();
    assert(tree != NULL);
    assert(tree->root == NULL);
    assert(tree->size == 0);
    assert(get_tree_size(tree) == 0);
    assert(get_tree_height(tree->root) == -1); // As per typical definition for empty tree
    destroy_binary_tree(tree);

    destroy_binary_tree(NULL); // Test destroying a NULL tree
    printf("  test_create_destroy_tree: PASSED\n");
}

void test_get_size(void) {
    printf("  Testing get_tree_size()...\n");
    BinaryTree *tree = create_binary_tree();
    assert(get_tree_size(tree) == 0);
    assert(tree->size == 0);

    insert_bst_node(tree, 10);
    assert(get_tree_size(tree) == 1);
    assert(tree->size == 1);

    insert_bst_node(tree, 5);
    assert(get_tree_size(tree) == 2);
    assert(tree->size == 2);
    
    // Test after potential deletions (if this function is called again)
    if (tree->size == 2) { // Assuming it's called after above inserts and before any deletes
        delete_bst_node(tree, 5);
        assert(get_tree_size(tree) == 1);
        assert(tree->size == 1);
        delete_bst_node(tree, 10);
        assert(get_tree_size(tree) == 0);
        assert(tree->size == 0);
    }


    destroy_binary_tree(tree);
    printf("  test_get_size: PASSED\n");
}


void test_bst_insert_and_search(void) {
    printf("  Testing BST insert and search...\n");
    BinaryTree *tree = create_binary_tree();
    assert(tree != NULL);

    // Test insertion into an empty tree
    printf("    Inserting 10 into empty tree...\n");
    assert(insert_bst_node(tree, 10) == 0); // 0 for success
    assert(tree->size == 1 && get_tree_size(tree) == 1);
    assert(tree->root != NULL && tree->root->data == 10);
    assert(search_bst_node(tree, 10) != NULL && search_bst_node(tree, 10)->data == 10);

    // Insert more unique keys
    printf("    Inserting 5, 15, 3, 7, 12, 17...\n");
    int keys_to_insert[] = {5, 15, 3, 7, 12, 17};
    int expected_size = 1;
    for (int i = 0; i < sizeof(keys_to_insert)/sizeof(keys_to_insert[0]); ++i) {
        assert(insert_bst_node(tree, keys_to_insert[i]) == 0);
        expected_size++;
        assert(tree->size == (size_t)expected_size && get_tree_size(tree) == (size_t)expected_size);
        assert(search_bst_node(tree, keys_to_insert[i]) != NULL && search_bst_node(tree, keys_to_insert[i])->data == keys_to_insert[i]);
    }

    // Attempt to insert duplicate keys
    printf("    Attempting to insert duplicates (10, 5)...\n");
    assert(insert_bst_node(tree, 10) == 1); // 1 for exists
    assert(tree->size == (size_t)expected_size && get_tree_size(tree) == (size_t)expected_size); // Size should not change

    assert(insert_bst_node(tree, 5) == 1); // 1 for exists
    assert(tree->size == (size_t)expected_size && get_tree_size(tree) == (size_t)expected_size);

    // Search for non-existent keys
    printf("    Searching for non-existent keys (100, 13)...\n");
    assert(search_bst_node(tree, 100) == NULL);
    assert(search_bst_node(tree, 13) == NULL);
    
    // Test insertion returning -1 (malloc error) - This is hard to test reliably without mocking malloc. Skip for now.

    destroy_binary_tree(tree);
    printf("  test_bst_insert_and_search: PASSED\n");
}

void test_bst_delete(void) {
    printf("  Testing BST delete...\n");
    BinaryTree *tree = create_binary_tree();

    // Build a known tree:
    //        10
    //       /  \
    //      5    15
    //     / \   / \
    //    3   7 12  17
    //   /     \      \
    //  1       8      20
    //                 /
    //                18

    insert_bst_node(tree, 10); insert_bst_node(tree, 5); insert_bst_node(tree, 15);
    insert_bst_node(tree, 3);  insert_bst_node(tree, 7); insert_bst_node(tree, 12); insert_bst_node(tree, 17);
    insert_bst_node(tree, 1);  insert_bst_node(tree, 8); insert_bst_node(tree, 20); insert_bst_node(tree, 18);
    size_t current_size = 11;
    assert(tree->size == current_size);

    // Delete Leaf Node (1)
    printf("    Deleting leaf node (1)...\n");
    assert(delete_bst_node(tree, 1) == 0); current_size--;
    assert(tree->size == current_size && search_bst_node(tree, 1) == NULL);
    assert(search_bst_node(tree, 3) != NULL); // Parent still there

    // Delete Node with One Right Child (7, its child 8)
    printf("    Deleting node with one right child (7)...\n");
    assert(delete_bst_node(tree, 7) == 0); current_size--;
    assert(tree->size == current_size && search_bst_node(tree, 7) == NULL);
    assert(search_bst_node(tree, 5)->right->data == 8); // 8 should replace 7 as child of 5

    // Delete Node with One Left Child (3, its child was 1, now NULL after 1 was deleted)
    // Let's re-insert 1 then 2, so 3 has left child 1, right child 2. Delete 2.
    insert_bst_node(tree,1); current_size++; // re-add 1
    insert_bst_node(tree,2); current_size++; // add 2, child of 1
    // Now tree around 3 is: 3(L:1(R:2))
    // Delete 1. 3's left child becomes 2.
    printf("    Deleting node with one child (1, its child 2)...\n");
    assert(delete_bst_node(tree,1) == 0); current_size--;
    assert(tree->size == current_size && search_bst_node(tree,1) == NULL);
    assert(search_bst_node(tree,3)->left->data == 2);


    // Delete Node with Two Children (15 -> successor is 17)
    // Current tree includes 15(L:12, R:17(R:20(L:18)))
    printf("    Deleting node with two children (15)...\n");
    assert(delete_bst_node(tree, 15) == 0); current_size--;
    assert(tree->size == current_size && search_bst_node(tree, 15) == NULL);
    assert(search_bst_node(tree, 10)->right->data == 17); // 17 (successor) should replace 15
    assert(search_bst_node(tree, 17)->right->data == 20); // Structure under 17 maintained
    assert(search_bst_node(tree, 17)->left->data == 12);  // Left child of original 15 is now left of 17

    // Delete Root Node (10 -> successor is 12)
    // Current root is 10. Its inorder successor is 12.
    printf("    Deleting root node (10)...\n");
    assert(delete_bst_node(tree, 10) == 0); current_size--;
    assert(tree->size == current_size && search_bst_node(tree, 10) == NULL);
    assert(tree->root->data == 12); // 12 should be new root

    // Attempt to delete non-existent key
    printf("    Attempting to delete non-existent key (100)...\n");
    assert(delete_bst_node(tree, 100) == 1); // 1 for not found
    assert(tree->size == current_size); // Size should not change

    // Delete all remaining nodes
    printf("    Deleting all remaining nodes...\n");
    int keys_in_tree[] = {5, 3, 2, 8, 12, 17, 20, 18}; // Keys remaining after specific deletes above
                                                      // Order of deletion might matter for testing specific cases,
                                                      // but here we just empty it.
    for(int i=0; i < sizeof(keys_in_tree)/sizeof(keys_in_tree[0]); ++i) {
        if (search_bst_node(tree, keys_in_tree[i])) { // Check if key exists before deleting
             // printf("Deleting %d, current size %zu\n", keys_in_tree[i], tree->size);
            assert(delete_bst_node(tree, keys_in_tree[i]) == 0);
            current_size--;
            assert(tree->size == current_size);
        }
    }
    // There might be an issue with the exact list of keys_in_tree if logic above had slight variations
    // A more robust way to empty:
    while(tree->root != NULL) {
        // printf("Deleting root %d to empty tree, current size %zu\n", tree->root->data, tree->size);
        assert(delete_bst_node(tree, tree->root->data) == 0);
        current_size--; // This should lead to 0
    }
    
    assert(tree->root == NULL && tree->size == 0 && get_tree_size(tree) == 0);

    destroy_binary_tree(tree);
    printf("  test_bst_delete: PASSED\n");
}


void test_traversals(void) {
    printf("  Testing traversals (inorder, preorder, postorder)...\n");
    BinaryTree *tree = create_binary_tree();

    printf("    Traversals on empty tree:\n");
    printf("      Inorder: "); inorder_traversal(tree->root); printf("(Expected: empty)\n");
    printf("      Preorder: "); preorder_traversal(tree->root); printf("(Expected: empty)\n");
    printf("      Postorder: "); postorder_traversal(tree->root); printf("(Expected: empty)\n");

    insert_bst_node(tree, 10);
    printf("    Traversals on single-node tree (10):\n");
    printf("      Inorder: "); inorder_traversal(tree->root); printf("(Expected: 10)\n");
    printf("      Preorder: "); preorder_traversal(tree->root); printf("(Expected: 10)\n");
    printf("      Postorder: "); postorder_traversal(tree->root); printf("(Expected: 10)\n");

    // Build tree: 10, 5, 15, 3, 7, 12, 17
    insert_bst_node(tree, 5); insert_bst_node(tree, 15);
    insert_bst_node(tree, 3); insert_bst_node(tree, 7);
    insert_bst_node(tree, 12); insert_bst_node(tree, 17);
    
    printf("    Traversals on tree (10, 5, 15, 3, 7, 12, 17):\n");
    // Inorder: 3 5 7 10 12 15 17
    // Preorder:10 5 3 7 15 12 17
    // Postorder:3 7 5 12 17 15 10
    printf("      Inorder: "); inorder_traversal(tree->root); printf("(Expected: 3 5 7 10 12 15 17)\n");
    printf("      Preorder: "); preorder_traversal(tree->root); printf("(Expected: 10 5 3 7 15 12 17)\n");
    printf("      Postorder: "); postorder_traversal(tree->root); printf("(Expected: 3 7 5 12 17 15 10)\n");
    
    // Note: Automated check for traversals is not implemented here due to stdout capture complexity.
    // This test primarily ensures they run without crashing and output can be visually inspected.
    // An inorder traversal check for sorted order could be done if traversals used a callback.

    destroy_binary_tree(tree);
    printf("  test_traversals: PASSED (visual check/no-crash)\n");
}

void test_get_height(void) {
    printf("  Testing get_tree_height()...\n");
    BinaryTree *tree = create_binary_tree();
    assert(get_tree_height(tree->root) == -1); // Empty tree

    insert_bst_node(tree, 10); // Root only
    assert(get_tree_height(tree->root) == 0);

    insert_bst_node(tree, 5);  // Skewed left or balanced
    assert(get_tree_height(tree->root) == 1);
    
    insert_bst_node(tree, 15); // Balanced
    assert(get_tree_height(tree->root) == 1);

    insert_bst_node(tree, 3);  // Left-Left
    assert(get_tree_height(tree->root) == 2);
    
    insert_bst_node(tree, 1);  // Left-Left-Left (skewed)
    assert(get_tree_height(tree->root) == 3);

    // Build a specific tree for height check
    destroy_binary_tree(tree);
    tree = create_binary_tree();
    //        10 (H=2)
    //       /  \
    //      5    15 (H=1)
    //     /      \
    //    3        17 (H=0)
    insert_bst_node(tree, 10); insert_bst_node(tree, 5); insert_bst_node(tree, 15);
    insert_bst_node(tree, 3); insert_bst_node(tree, 17);
    assert(get_tree_height(tree->root) == 2);
    assert(get_tree_height(search_bst_node(tree,5)) == 1);
    assert(get_tree_height(search_bst_node(tree,15)) == 1);
    assert(get_tree_height(search_bst_node(tree,3)) == 0);
    assert(get_tree_height(search_bst_node(tree,17)) == 0);
    assert(get_tree_height(search_bst_node(tree,12345)) == -1); // Non-existent node


    destroy_binary_tree(tree);
    printf("  test_get_height: PASSED\n");
}

void test_find_min_max(void) {
    printf("  Testing find_min_node() and find_max_node()...\n");
    BinaryTree *tree = create_binary_tree();
    assert(find_min_node(tree->root) == NULL); // Empty tree
    assert(find_max_node(tree->root) == NULL); // Empty tree

    insert_bst_node(tree, 10); // Single node
    assert(find_min_node(tree->root)->data == 10);
    assert(find_max_node(tree->root)->data == 10);

    insert_bst_node(tree, 5); insert_bst_node(tree, 15);
    insert_bst_node(tree, 3); insert_bst_node(tree, 7);
    insert_bst_node(tree, 12); insert_bst_node(tree, 17);
    // Tree: 10 (L:5(L:3, R:7), R:15(L:12, R:17))
    assert(find_min_node(tree->root)->data == 3);
    assert(find_max_node(tree->root)->data == 17);

    delete_bst_node(tree, 3); // Delete min
    assert(find_min_node(tree->root)->data == 5);
    delete_bst_node(tree, 17); // Delete max
    assert(find_max_node(tree->root)->data == 15);
    
    // Delete until only root left
    delete_bst_node(tree, 5); delete_bst_node(tree, 7);
    delete_bst_node(tree, 12); delete_bst_node(tree, 15);
    assert(find_min_node(tree->root)->data == 10);
    assert(find_max_node(tree->root)->data == 10);

    delete_bst_node(tree, 10); // Empty tree again
    assert(find_min_node(tree->root) == NULL);
    assert(find_max_node(tree->root) == NULL);

    destroy_binary_tree(tree);
    printf("  test_find_min_max: PASSED\n");
}
