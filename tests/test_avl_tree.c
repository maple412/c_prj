#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <assert.h>
#include <limits.h> // For INT_MIN, INT_MAX in property checker

#include "../src/avl_tree.h" 

// Helper function prototypes for property checking
bool check_avl_properties(AVLTree* tree);
static int compute_node_height_for_test(AVLNode* node); 
static bool check_avl_properties_recursive(AVLNode* node, int min_key_val, int max_key_val); // Removed unused min/max pointers


// Test function prototypes
void test_create_destroy_avltree(void);
void test_avl_insert_simple_no_rotation(void);
void test_avl_insert_rotations(void); 
void test_avl_delete_simple_no_rotation(void);
void test_avl_delete_rotations(void); 
void test_avl_search(void);
void test_avl_properties_after_many_ops(void); 

// Main function to run all tests
int main() {
    printf("Running AVL Tree tests...\n\n");

    test_create_destroy_avltree();
    test_avl_search(); 
    test_avl_insert_simple_no_rotation();
    test_avl_insert_rotations();
    test_avl_delete_simple_no_rotation(); 
    test_avl_delete_rotations(); 
    test_avl_properties_after_many_ops(); 

    printf("\nAll AVL Tree tests completed!\n");
    return 0;
}

// --- AVL Property Checker Implementation ---

static int compute_node_height_for_test(AVLNode* node) {
    if (node == NULL) return -1;
    int left_h = compute_node_height_for_test(node->left);
    int right_h = compute_node_height_for_test(node->right);
    return 1 + (left_h > right_h ? left_h : right_h);
}

// Updated to use value parameters for min/max key checks for simplicity
static bool check_avl_properties_recursive(AVLNode* node, int min_key_val, int max_key_val) {
    if (node == NULL) {
        return true;
    }

    // Check BST property
    if (node->key <= min_key_val || node->key >= max_key_val) {
        printf("BST Property Violated: Node %d is not within range (%d, %d).\n", node->key, min_key_val, max_key_val);
        return false;
    }

    // Check balance factor
    int left_h = compute_node_height_for_test(node->left);
    int right_h = compute_node_height_for_test(node->right);
    int balance_factor = left_h - right_h;

    if (balance_factor < -1 || balance_factor > 1) {
        printf("AVL Balance Factor Violated: Node %d has balance factor %d (L:%d, R:%d).\n", node->key, balance_factor, left_h, right_h);
        return false;
    }
    
    // Check if the stored height in the node matches the computed height
    if (node->height != (1 + (left_h > right_h ? left_h : right_h)) ) {
         printf("AVL Height Mismatch: Node %d stored height %d, computed height %d (L:%d, R:%d).\n",
                node->key, node->height, (1 + (left_h > right_h ? left_h : right_h)), left_h, right_h);
         return false;
    }

    // Recursively check left and right subtrees
    if (!check_avl_properties_recursive(node->left, min_key_val, node->key)) return false;
    if (!check_avl_properties_recursive(node->right, node->key, max_key_val)) return false;

    return true;
}

bool check_avl_properties(AVLTree* tree) {
    if (tree == NULL) {
        return true; 
    }
    if (tree->root == NULL) {
        if (tree->size != 0) {
            printf("Property Check Error: Tree root is NULL but size is %zu.\n", tree->size);
            return false;
        }
        return true; // Empty tree is valid
    }
    // Use INT_MIN and INT_MAX for initial bounds
    return check_avl_properties_recursive(tree->root, INT_MIN, INT_MAX);
}


// --- Test Function Implementations ---

void test_create_destroy_avltree(void) {
    printf("  Testing AVL tree creation and destruction...\n");
    AVLTree *tree = create_avl_tree();
    assert(tree != NULL);
    assert(tree->root == NULL);
    assert(tree->size == 0);
    assert(get_avl_tree_size(tree) == 0);
    assert(get_avl_tree_height(tree) == -1); 
    assert(check_avl_properties(tree) == true); 
    destroy_avl_tree(tree);

    destroy_avl_tree(NULL); 
    printf("  test_create_destroy_avltree: PASSED\n");
}

void test_avl_search(void) {
    printf("  Testing AVL tree search...\n");
    AVLTree *tree = create_avl_tree();
    assert(search_in_avl_tree(tree, 100) == NULL); 

    insert_into_avl_tree(tree, 10);
    insert_into_avl_tree(tree, 5);
    insert_into_avl_tree(tree, 15);
    insert_into_avl_tree(tree, 3); 
    insert_into_avl_tree(tree, 7); 
    assert(check_avl_properties(tree) == true);


    assert(search_in_avl_tree(tree, 10) != NULL && search_in_avl_tree(tree, 10)->key == 10);
    assert(search_in_avl_tree(tree, 3) != NULL && search_in_avl_tree(tree, 3)->key == 3);
    assert(search_in_avl_tree(tree, 7) != NULL && search_in_avl_tree(tree, 7)->key == 7);
    assert(search_in_avl_tree(tree, 15) != NULL && search_in_avl_tree(tree, 15)->key == 15);
    assert(search_in_avl_tree(tree, 5) != NULL && search_in_avl_tree(tree, 5)->key == 5);
    assert(search_in_avl_tree(tree, 100) == NULL); 
    assert(search_in_avl_tree(tree, 1) == NULL);

    destroy_avl_tree(tree);
    printf("  test_avl_search: PASSED\n");
}

void test_avl_insert_simple_no_rotation(void) {
    printf("  Testing AVL tree simple insertions (no rotations expected)...\n");
    AVLTree *tree = create_avl_tree();
    assert(tree != NULL);

    assert(insert_into_avl_tree(tree, 10) == 0); 
    assert(tree->root->key == 10 && tree->root->height == 0 && tree->size == 1);
    assert(get_avl_tree_size(tree) == 1 && get_avl_tree_height(tree) == 0);
    assert(check_avl_properties(tree) == true);

    assert(insert_into_avl_tree(tree, 5) == 0);
    assert(tree->size == 2 && tree->root->height == 1); 
    assert(get_avl_tree_size(tree) == 2 && get_avl_tree_height(tree) == 1);
    assert(check_avl_properties(tree) == true);

    assert(insert_into_avl_tree(tree, 15) == 0);
    assert(tree->size == 3 && tree->root->height == 1); 
    assert(get_avl_tree_size(tree) == 3 && get_avl_tree_height(tree) == 1);
    assert(check_avl_properties(tree) == true);

    assert(insert_into_avl_tree(tree, 10) == 1); 
    assert(tree->size == 3); 
    assert(check_avl_properties(tree) == true);

    destroy_avl_tree(tree);
    printf("  test_avl_insert_simple_no_rotation: PASSED\n");
}

void test_avl_insert_rotations(void) {
    printf("  Testing AVL tree insertions with rotations...\n");
    AVLTree *tree;

    // Left-Left Case
    tree = create_avl_tree();
    printf("    Testing LL Case (insert 30, 20, 10)...\n");
    assert(insert_into_avl_tree(tree, 30) == 0);
    assert(insert_into_avl_tree(tree, 20) == 0);
    assert(check_avl_properties(tree) == true); 
    assert(insert_into_avl_tree(tree, 10) == 0); 
    assert(tree->root->key == 20 && tree->root->left->key == 10 && tree->root->right->key == 30);
    assert(tree->size == 3 && get_avl_tree_height(tree) == 1);
    assert(check_avl_properties(tree) == true);
    destroy_avl_tree(tree);

    // Right-Right Case
    tree = create_avl_tree();
    printf("    Testing RR Case (insert 10, 20, 30)...\n");
    assert(insert_into_avl_tree(tree, 10) == 0);
    assert(insert_into_avl_tree(tree, 20) == 0);
    assert(check_avl_properties(tree) == true);
    assert(insert_into_avl_tree(tree, 30) == 0); 
    assert(tree->root->key == 20 && tree->root->left->key == 10 && tree->root->right->key == 30);
    assert(tree->size == 3 && get_avl_tree_height(tree) == 1);
    assert(check_avl_properties(tree) == true);
    destroy_avl_tree(tree);

    // Left-Right Case
    tree = create_avl_tree();
    printf("    Testing LR Case (insert 30, 10, 20)...\n");
    assert(insert_into_avl_tree(tree, 30) == 0);
    assert(insert_into_avl_tree(tree, 10) == 0);
    assert(check_avl_properties(tree) == true);
    assert(insert_into_avl_tree(tree, 20) == 0); 
    assert(tree->root->key == 20 && tree->root->left->key == 10 && tree->root->right->key == 30);
    assert(tree->size == 3 && get_avl_tree_height(tree) == 1);
    assert(check_avl_properties(tree) == true);
    destroy_avl_tree(tree);

    // Right-Left Case
    tree = create_avl_tree();
    printf("    Testing RL Case (insert 10, 30, 20)...\n");
    assert(insert_into_avl_tree(tree, 10) == 0);
    assert(insert_into_avl_tree(tree, 30) == 0);
    assert(check_avl_properties(tree) == true);
    assert(insert_into_avl_tree(tree, 20) == 0); 
    assert(tree->root->key == 20 && tree->root->left->key == 10 && tree->root->right->key == 30);
    assert(tree->size == 3 && get_avl_tree_height(tree) == 1);
    assert(check_avl_properties(tree) == true);
    destroy_avl_tree(tree);

    // More complex sequence: 10, 20, 30 (RR), 5 (balance), 3 (LL at 10 then balance), 15 (balance), 25 (balance), 35 (RR at 30 then balance)
    printf("    Testing more complex sequence...\n");
    tree = create_avl_tree();
    int values[] = {10, 20, 30, 5, 3, 15, 25, 35, 12, 17, 22, 27, 32, 37, 1};
    for(int i=0; i < sizeof(values)/sizeof(values[0]); ++i) {
        // printf("Inserting %d\n", values[i]);
        assert(insert_into_avl_tree(tree, values[i]) == 0);
        assert(check_avl_properties(tree) == true);
        // inorder_avl_traversal(tree->root); printf("\n");
    }
    assert(tree->size == sizeof(values)/sizeof(values[0]));
    // printf("Final height for complex insert: %d\n", get_avl_tree_height(tree));
    destroy_avl_tree(tree);

    printf("  test_avl_insert_rotations: PASSED\n");
}

void test_avl_delete_simple_no_rotation(void) {
    printf("  Testing AVL delete simple (no rotation needed)...\n");
    AVLTree *tree = create_avl_tree();

    // Tree: 20(L:10, R:30)
    insert_into_avl_tree(tree, 20); insert_into_avl_tree(tree, 10); insert_into_avl_tree(tree, 30);
    assert(check_avl_properties(tree) == true && tree->size == 3);
    
    // Delete 10 (leaf)
    assert(delete_from_avl_tree(tree, 10) == 0); // 0 for success
    assert(tree->size == 2 && search_in_avl_tree(tree, 10) == NULL);
    assert(get_avl_tree_height(tree) == 1); // Height of 20(R:30) is 1
    assert(check_avl_properties(tree) == true);

    // Delete 30 (leaf)
    assert(delete_from_avl_tree(tree, 30) == 0);
    assert(tree->size == 1 && search_in_avl_tree(tree, 30) == NULL);
    assert(get_avl_tree_height(tree) == 0); // Height of 20 is 0
    assert(check_avl_properties(tree) == true);

    // Delete 20 (root)
    assert(delete_from_avl_tree(tree, 20) == 0);
    assert(tree->size == 0 && tree->root == NULL);
    assert(get_avl_tree_height(tree) == -1);
    assert(check_avl_properties(tree) == true);

    // Tree: 20(L:10(R:15), R:30)
    insert_into_avl_tree(tree, 20); insert_into_avl_tree(tree, 10); insert_into_avl_tree(tree, 30); insert_into_avl_tree(tree, 15);
    assert(check_avl_properties(tree) == true && tree->size == 4);
    // Delete 15 (leaf, no rotation)
    assert(delete_from_avl_tree(tree, 15) == 0);
    assert(tree->size == 3 && search_in_avl_tree(tree, 15) == NULL);
    assert(get_avl_tree_height(tree) == 1); // Root 20, L:10, R:30
    assert(check_avl_properties(tree) == true);

    // Delete node with one child (e.g. delete 10, 15 replaces it)
    // Build 20, 10, 30, 5 -> Root 20, L:10(L:5), R:30
    destroy_avl_tree(tree); tree = create_avl_tree();
    insert_into_avl_tree(tree, 20); insert_into_avl_tree(tree, 10); insert_into_avl_tree(tree, 30); insert_into_avl_tree(tree, 5);
    assert(check_avl_properties(tree) == true && tree->size == 4);
    assert(delete_from_avl_tree(tree, 10) == 0); // 5 takes 10's place
    assert(tree->size == 3 && search_in_avl_tree(tree, 10) == NULL);
    assert(tree->root->left->key == 5);
    assert(check_avl_properties(tree) == true);


    destroy_avl_tree(tree);
    printf("  test_avl_delete_simple_no_rotation: PASSED\n");
}

void test_avl_delete_rotations(void) {
    printf("  Testing AVL delete with rotations...\n");
    AVLTree *tree;

    // Deletion leading to Right Rotation (LL case for rebalance)
    tree = create_avl_tree();
    // Build: 20(B), 10(B, L:5(R)), 30(B) -> (after insertions 20,10,30,5)
    insert_into_avl_tree(tree, 20); insert_into_avl_tree(tree, 10); insert_into_avl_tree(tree, 30); insert_into_avl_tree(tree, 5);
    assert(check_avl_properties(tree) == true);
    // Delete 30. Node 20 becomes imbalanced (BF=2). Node 10 is left child (BF=1 for 10). LL case.
    // Right rotate 20. New root 10. 10(L:5, R:20)
    printf("    Testing delete leading to Right Rotation (LL rebalance)...\n");
    assert(delete_from_avl_tree(tree, 30) == 0);
    assert(tree->size == 3 && search_in_avl_tree(tree, 30) == NULL);
    assert(tree->root->key == 10);
    assert(tree->root->left->key == 5 && tree->root->right->key == 20);
    assert(check_avl_properties(tree) == true);
    destroy_avl_tree(tree);

    // Deletion leading to Left Rotation (RR case for rebalance)
    tree = create_avl_tree();
    // Build: 10(B), 5(B), 20(B, R:25(R)) -> (after insertions 10,5,20,25)
    insert_into_avl_tree(tree, 10); insert_into_avl_tree(tree, 5); insert_into_avl_tree(tree, 20); insert_into_avl_tree(tree, 25);
    assert(check_avl_properties(tree) == true);
    // Delete 5. Node 10 becomes imbalanced (BF=-2). Node 20 is right child (BF=-1 for 20). RR case.
    // Left rotate 10. New root 20. 20(L:10, R:25)
    printf("    Testing delete leading to Left Rotation (RR rebalance)...\n");
    assert(delete_from_avl_tree(tree, 5) == 0);
    assert(tree->size == 3 && search_in_avl_tree(tree, 5) == NULL);
    assert(tree->root->key == 20);
    assert(tree->root->left->key == 10 && tree->root->right->key == 25);
    assert(check_avl_properties(tree) == true);
    destroy_avl_tree(tree);

    // Deletion leading to LR rotation
    tree = create_avl_tree();
    // Build: 20(B), 10(B, R:15(R)), 30(B) -> (after 20,10,30,15)
    insert_into_avl_tree(tree, 20); insert_into_avl_tree(tree, 10); insert_into_avl_tree(tree, 30); insert_into_avl_tree(tree, 15);
    assert(check_avl_properties(tree) == true);
    // Delete 30. Node 20 becomes imbalanced (BF=2). Node 10 is L child (BF=-1 for 10 due to 15). LR case.
    // Left rotate 10, then Right rotate 20. New root 15. 15(L:10, R:20)
    printf("    Testing delete leading to LR Rotation...\n");
    assert(delete_from_avl_tree(tree, 30) == 0);
    assert(tree->size == 3 && search_in_avl_tree(tree, 30) == NULL);
    assert(tree->root->key == 15);
    assert(tree->root->left->key == 10 && tree->root->right->key == 20);
    assert(check_avl_properties(tree) == true);
    destroy_avl_tree(tree);

    // Deletion leading to RL rotation
    tree = create_avl_tree();
    // Build: 10(B), 5(B), 20(B, L:15(R)) -> (after 10,5,20,15)
    insert_into_avl_tree(tree, 10); insert_into_avl_tree(tree, 5); insert_into_avl_tree(tree, 20); insert_into_avl_tree(tree, 15);
    assert(check_avl_properties(tree) == true);
    // Delete 5. Node 10 becomes imbalanced (BF=-2). Node 20 is R child (BF=1 for 20 due to 15). RL case.
    // Right rotate 20, then Left rotate 10. New root 15. 15(L:10, R:20)
    printf("    Testing delete leading to RL Rotation...\n");
    assert(delete_from_avl_tree(tree, 5) == 0);
    assert(tree->size == 3 && search_in_avl_tree(tree, 5) == NULL);
    assert(tree->root->key == 15);
    assert(tree->root->left->key == 10 && tree->root->right->key == 20);
    assert(check_avl_properties(tree) == true);
    destroy_avl_tree(tree);

    printf("  test_avl_delete_rotations: PASSED\n");
}

void test_avl_properties_after_many_ops(void) {
    printf("  Testing AVL properties after many mixed operations (Stress Test)...\n");
    AVLTree *tree = create_avl_tree();
    const int num_ops = 500; // Increased ops
    const int max_val = 2000;
    int* inserted_keys = (int*)malloc(sizeof(int) * num_ops);
    int current_key_count = 0;

    if (inserted_keys == NULL) {
        perror("Failed to allocate for stress test key tracking");
        destroy_avl_tree(tree);
        return;
    }

    for (int i = 0; i < num_ops; ++i) {
        int op_type = rand() % 3; // 0 for insert, 1 for delete, 2 for search
        int value = rand() % max_val;

        if (op_type == 0 || current_key_count < 20) { // Insert, or if tree is small
            // printf("Op %d: Inserting %d\n", i, value);
            if (insert_into_avl_tree(tree, value) == 0) { // Successfully inserted
                bool exists = false; // Check if already in our tracking array
                for(int k=0; k<current_key_count; ++k) if(inserted_keys[k] == value) exists = true;
                if(!exists) inserted_keys[current_key_count++] = value;
            }
        } else if (op_type == 1 && current_key_count > 0) { // Delete
            int del_idx = rand() % current_key_count;
            value = inserted_keys[del_idx];
            // printf("Op %d: Deleting %d\n", i, value);
            if (delete_from_avl_tree(tree, value) == 0) { // Successfully deleted
                inserted_keys[del_idx] = inserted_keys[--current_key_count]; // Remove from tracking array
            }
        } else { // Search
            // printf("Op %d: Searching %d\n", i, value);
            search_in_avl_tree(tree, value); 
        }
        
        if (i % 50 == 0 || i == num_ops - 1) { 
            // printf("  Stress Test: Current Size: %zu\n", tree->size);
            // inorder_avl_traversal(tree->root); printf("\n");
            assert(check_avl_properties(tree) == true);
            assert(tree->size == (size_t)current_key_count); // Verify size consistency
        }
    }
    assert(check_avl_properties(tree) == true);
    assert(tree->size == (size_t)current_key_count);

    free(inserted_keys);
    destroy_avl_tree(tree);
    printf("  test_avl_properties_after_many_ops: PASSED\n");
}
