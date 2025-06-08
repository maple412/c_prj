#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h> // For INT_MIN, INT_MAX in key range checks

#include "../src/btree.h"

// --- Helper function prototypes for property checking ---
bool check_btree_properties(BTree* tree);
static bool check_btree_properties_recursive(BTreeNode* node, int t, int depth, int* leaf_depth, bool is_root, int min_key_val, int max_key_val);
static int get_subtree_min_key(BTreeNode* node);
static int get_subtree_max_key(BTreeNode* node);

// --- Test function prototypes ---
void test_create_destroy_btree(void);
void test_btree_insert_simple(void);
void test_btree_insert_splitting(void);
void test_btree_search(void);
void test_btree_delete_simple_leaf(void);
void test_btree_delete_complex_merge_borrow(void);
void test_btree_properties_after_ops(void);

// --- Main function to run all tests ---
int main() {
    printf("Running B-Tree tests...\n\n");

    test_create_destroy_btree();
    test_btree_search(); // Test search early with empty/simple trees
    test_btree_insert_simple();
    test_btree_insert_splitting();
    test_btree_delete_simple_leaf();
    test_btree_delete_complex_merge_borrow();
    test_btree_properties_after_ops();

    printf("\nAll B-Tree tests completed!\n");
    return 0;
}

// --- B-Tree Property Checker Implementation ---

static int get_subtree_min_key(BTreeNode* node) {
    if (!node) return INT_MAX; // Should not happen with valid BTree
    while (!node->is_leaf) {
        node = node->children[0];
    }
    return (node->num_keys > 0) ? node->keys[0] : INT_MAX;
}

static int get_subtree_max_key(BTreeNode* node) {
    if (!node) return INT_MIN; // Should not happen
    while (!node->is_leaf) {
        node = node->children[node->num_keys];
    }
    return (node->num_keys > 0) ? node->keys[node->num_keys - 1] : INT_MIN;
}

static bool check_btree_properties_recursive(BTreeNode* node, int t, int depth, int* leaf_depth, bool is_root, int min_key_val, int max_key_val) {
    if (node == NULL) {
        // This case should ideally not be reached if children pointers are managed correctly.
        // An internal node should not have NULL children pointers where children are expected.
        printf("Error: Encountered NULL node during recursive check (should be handled by caller or structure).\n");
        return false;
    }

    // 1. Key Count Check
    if (is_root) {
        if (node->num_keys == 0) { // Empty tree (root is a leaf with 0 keys)
            if (!node->is_leaf) {
                printf("Property Violation: Root has 0 keys but is not a leaf.\n");
                return false;
            }
            // If root is a leaf with 0 keys, it's a valid empty tree. No further checks needed for this node.
        } else if (node->num_keys < 1 || node->num_keys > (2 * t - 1)) {
            printf("Property Violation: Root node key count %d out of range [1, %d].\n", node->num_keys, (2 * t - 1));
            return false;
        }
    } else { // Non-root node
        if (node->num_keys < (t - 1) || node->num_keys > (2 * t - 1)) {
            printf("Property Violation: Non-root node key count %d out of range [%d, %d].\n", node->num_keys, (t - 1), (2 * t - 1));
            return false;
        }
    }

    // 2. Keys are Sorted
    for (int i = 0; i < node->num_keys - 1; i++) {
        if (node->keys[i] >= node->keys[i + 1]) {
            printf("Property Violation: Keys not sorted in node. keys[%d]=%d, keys[%d+1]=%d.\n", i, node->keys[i], i, node->keys[i+1]);
            return false;
        }
    }
    
    // 3. All keys within min_key_val and max_key_val range (from parent)
    for (int i = 0; i < node->num_keys; i++) {
        if (node->keys[i] <= min_key_val || node->keys[i] >= max_key_val) {
            printf("Property Violation: Key %d out of range (%d, %d) defined by parent/ancestor.\n", node->keys[i], min_key_val, max_key_val);
            return false;
        }
    }


    if (node->is_leaf) {
        // 4. All Leaves at Same Depth
        if (*leaf_depth == -1) {
            *leaf_depth = depth;
        } else if (*leaf_depth != depth) {
            printf("Property Violation: Leaves at different depths. Expected %d, got %d.\n", *leaf_depth, depth);
            return false;
        }
    } else { // Internal node
        // 5. Number of Children for Internal Node
        if (node->num_keys == 0 && !is_root) { // Non-root empty internal node is not allowed
             printf("Property Violation: Non-root internal node has 0 keys.\n"); return false;
        }
        // An internal node must have num_keys + 1 children
        // Check first num_keys + 1 children are not NULL
        for(int i = 0; i <= node->num_keys; ++i) {
            if (node->children[i] == NULL) {
                printf("Property Violation: Internal node missing child at index %d (expected %d children).\n", i, node->num_keys + 1);
                return false;
            }
        }


        // Recursively check children and their key ranges
        for (int i = 0; i <= node->num_keys; i++) {
            int child_min_key = (i == 0) ? min_key_val : node->keys[i - 1];
            int child_max_key = (i == node->num_keys) ? max_key_val : node->keys[i];
            
            if (!check_btree_properties_recursive(node->children[i], t, depth + 1, leaf_depth, false, child_min_key, child_max_key)) {
                return false;
            }
        }
    }
    return true;
}

bool check_btree_properties(BTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        fprintf(stderr, "Tree or root is NULL. Cannot check properties.\n");
        return false; // Or true if an empty/NULL tree is considered valid by convention
    }
    if (tree->t < 2) {
         fprintf(stderr, "Invalid B-Tree degree t=%d. Must be >= 2.\n", tree->t);
        return false;
    }

    int leaf_depth = -1;
    // Root properties (key count, children if not leaf) are handled by the first call to recursive helper.
    // If root is not leaf and has 0 keys, it must have exactly one child (which becomes new root - this state should be temporary during delete).
    // The recursive check handles this: if root is not leaf and num_keys=0, it's an error unless it's the only node (empty tree).
    // If tree->root->num_keys == 0 and !tree->root->is_leaf, this means tree height should shrink.
    // The current check_btree_properties_recursive handles this: if root is not leaf, num_keys must be >=1.
    // Exception: if root is leaf, num_keys can be 0 (empty tree).
    if (tree->root->num_keys == 0 && !tree->root->is_leaf) {
        printf("Property Violation: Root has 0 keys and is not a leaf, but tree is not empty (should have shrunk height).\n");
        return false;
    }


    return check_btree_properties_recursive(tree->root, tree->t, 0, &leaf_depth, true, INT_MIN, INT_MAX);
}

// --- Test Function Implementations ---

void test_create_destroy_btree(void) {
    printf("  Testing B-Tree creation and destruction (t=2, t=3)...\n");
    BTree *tree_t2 = create_btree(2);
    assert(tree_t2 != NULL);
    assert(tree_t2->root != NULL);
    assert(tree_t2->t == 2);
    assert(tree_t2->root->is_leaf == true);
    assert(tree_t2->root->num_keys == 0);
    assert(check_btree_properties(tree_t2) == true); // Empty tree
    destroy_btree(tree_t2);

    BTree *tree_t3 = create_btree(3);
    assert(tree_t3 != NULL);
    assert(tree_t3->root != NULL);
    assert(tree_t3->t == 3);
    assert(check_btree_properties(tree_t3) == true); // Empty tree
    destroy_btree(tree_t3);
    
    BTree *tree_invalid_t = create_btree(1);
    assert(tree_invalid_t == NULL);

    destroy_btree(NULL); // Should not crash

    printf("  test_create_destroy_btree: PASSED\n");
}

void test_btree_search(void) {
    printf("  Testing B-Tree search (t=2)...\n");
    BTree *tree = create_btree(2);
    assert(search_btree(tree, 100) == false); // Search in empty tree

    insert_btree(tree, 10);
    assert(search_btree(tree, 10) == true);
    assert(search_btree(tree, 20) == false);

    insert_btree(tree, 20);
    insert_btree(tree, 5); // Root: [5, 10, 20]
    assert(check_btree_properties(tree) == true);


    insert_btree(tree, 15); // Split: Root [10], L[5], R[15,20]
    assert(check_btree_properties(tree) == true);
    insert_btree(tree, 25); // Root [10], L[5], R[15,20,25]
    assert(check_btree_properties(tree) == true);


    assert(search_btree(tree, 10) == true);
    assert(search_btree(tree, 5) == true);
    assert(search_btree(tree, 20) == true);
    assert(search_btree(tree, 15) == true);
    assert(search_btree(tree, 25) == true);
    assert(search_btree(tree, 100) == false); 
    assert(search_btree(tree, 12) == false);
    assert(search_btree(tree, 1) == false);

    destroy_btree(tree);
    printf("  test_btree_search: PASSED\n");
}

void test_btree_insert_simple(void) {
    printf("  Testing B-Tree simple insertions (no splits initially, t=2)...\n");
    BTree *tree = create_btree(2); // Max keys = 3
    assert(tree != NULL);

    insert_btree(tree, 10);
    assert(tree->root->num_keys == 1 && tree->root->keys[0] == 10);
    assert(search_btree(tree, 10) == true);
    assert(check_btree_properties(tree) == true);

    insert_btree(tree, 20);
    assert(tree->root->num_keys == 2 && tree->root->keys[0] == 10 && tree->root->keys[1] == 20);
    assert(search_btree(tree, 20) == true);
    assert(check_btree_properties(tree) == true);

    insert_btree(tree, 5); 
    assert(tree->root->num_keys == 3 && tree->root->keys[0] == 5 && tree->root->keys[1] == 10 && tree->root->keys[2] == 20);
    assert(search_btree(tree, 5) == true);
    assert(check_btree_properties(tree) == true);
    
    destroy_btree(tree);
    printf("  test_btree_insert_simple: PASSED\n");
}

void test_btree_insert_splitting(void) {
    printf("  Testing B-Tree insertions that cause splits (t=2, t=3)...\n");
    BTree *tree_t2 = create_btree(2); // Max keys = 3

    // Sequence for t=2 from prompt
    insert_btree(tree_t2, 10); insert_btree(tree_t2, 20); insert_btree(tree_t2, 5); // Root [5,10,20]
    assert(check_btree_properties(tree_t2) == true && tree_t2->root->num_keys == 3);
    
    insert_btree(tree_t2, 15); // Split: Root[10], L[5], R[15,20]
    assert(check_btree_properties(tree_t2) == true);
    assert(tree_t2->root->num_keys == 1 && tree_t2->root->keys[0] == 10);
    assert(!tree_t2->root->is_leaf);
    assert(tree_t2->root->children[0]->num_keys == 1 && tree_t2->root->children[0]->keys[0] == 5);
    assert(tree_t2->root->children[1]->num_keys == 2 && tree_t2->root->children[1]->keys[0] == 15 && tree_t2->root->children[1]->keys[1] == 20);

    insert_btree(tree_t2, 3); // To L child: L[3,5]
    assert(check_btree_properties(tree_t2) == true);
    assert(tree_t2->root->children[0]->num_keys == 2 && tree_t2->root->children[0]->keys[0] == 3);

    insert_btree(tree_t2, 7); // L child [3,5,7] splits, 5 to root. Root[5,10], C0[3], C1[7], C2[15,20]
    assert(check_btree_properties(tree_t2) == true);
    assert(tree_t2->root->num_keys == 2 && tree_t2->root->keys[0] == 5 && tree_t2->root->keys[1] == 10);
    assert(tree_t2->root->children[0]->num_keys == 1 && tree_t2->root->children[0]->keys[0] == 3);
    assert(tree_t2->root->children[1]->num_keys == 1 && tree_t2->root->children[1]->keys[0] == 7);
    assert(tree_t2->root->children[2]->num_keys == 2 && tree_t2->root->children[2]->keys[0] == 15);

    insert_btree(tree_t2, 30); // To C2: C2[15,20,30]
    assert(check_btree_properties(tree_t2) == true);
    assert(tree_t2->root->children[2]->num_keys == 3 && tree_t2->root->children[2]->keys[2] == 30);
    
    insert_btree(tree_t2, 40); // C2 [15,20,30,40] splits, 20 to root. Root[5,10,20], C0[3],C1[7],C2[15],C3[30,40]
    assert(check_btree_properties(tree_t2) == true);
    assert(tree_t2->root->num_keys == 3 && tree_t2->root->keys[2] == 20);
    assert(tree_t2->root->children[2]->num_keys == 1 && tree_t2->root->children[2]->keys[0] == 15);
    assert(tree_t2->root->children[3]->num_keys == 2 && tree_t2->root->children[3]->keys[0] == 30);

    assert(search_btree(tree_t2, 7)); assert(search_btree(tree_t2, 40)); assert(search_btree(tree_t2, 3));
    destroy_btree(tree_t2);

    // Test with t=3 (max keys 5, min keys 2)
    BTree *tree_t3 = create_btree(3);
    for (int i = 1; i <= 20; ++i) { // Insert 20 keys
        insert_btree(tree_t3, i * 10);
        assert(check_btree_properties(tree_t3) == true);
    }
    assert(search_btree(tree_t3, 50)); assert(search_btree(tree_t3, 150)); assert(!search_btree(tree_t3, 55));
    // print_btree(tree_t3); // Manual inspection if needed
    destroy_btree(tree_t3);

    printf("  test_btree_insert_splitting: PASSED\n");
}

void test_btree_delete_simple_leaf(void) {
    printf("  Testing B-Tree simple leaf deletions (no underflow, t=2)...\n");
    BTree *tree = create_btree(2); // Min keys = 1

    // Setup: Root [10], L[3,5], R[15,20,25] (after some insertions)
    insert_btree(tree, 10); insert_btree(tree, 5); insert_btree(tree, 20);
    insert_btree(tree, 3); insert_btree(tree, 15); insert_btree(tree, 25);
    assert(check_btree_properties(tree) == true);
    // print_btree(tree); // Root[10], L[3,5], R[15,20,25]

    // Delete 3 from leaf L (node [3,5] becomes [5], still >= t-1 keys)
    delete_btree(tree, 3);
    assert(search_btree(tree, 3) == false);
    assert(tree->root->children[0]->num_keys == 1 && tree->root->children[0]->keys[0] == 5);
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);

    // Delete 25 from leaf R (node [15,20,25] becomes [15,20], still >= t-1 keys)
    delete_btree(tree, 25);
    assert(search_btree(tree, 25) == false);
    assert(tree->root->children[1]->num_keys == 2 && tree->root->children[1]->keys[1] == 20);
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);
    
    // Delete from root when root is leaf and no underflow
    BTree* tree2 = create_btree(2);
    insert_btree(tree2, 10); insert_btree(tree2, 20); // Root [10,20]
    delete_btree(tree2, 10);
    assert(search_btree(tree2, 10) == false && tree2->root->num_keys == 1 && tree2->root->keys[0] == 20);
    assert(check_btree_properties(tree2) == true);
    delete_btree(tree2, 20); // Empty tree
    assert(search_btree(tree2, 20) == false && tree2->root->num_keys == 0);
    assert(check_btree_properties(tree2) == true);

    destroy_btree(tree);
    destroy_btree(tree2);
    printf("  test_btree_delete_simple_leaf: PASSED\n");
}

void test_btree_delete_complex_merge_borrow(void) {
    printf("  Testing B-Tree complex deletions (merge/borrow, t=2)...\n");
    BTree *tree;

    // Case: Borrow from right sibling (leaf)
    tree = create_btree(2);
    // Setup: Root[10], L[5], R[15,20]
    insert_btree(tree, 10); insert_btree(tree, 5); insert_btree(tree, 15); insert_btree(tree, 20);
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);
    delete_btree(tree, 5); // L child [5] underflows, borrows from R[15,20] via parent [10]
                           // Parent becomes [15], L[10], R[20]
    assert(search_btree(tree, 5) == false);
    assert(check_btree_properties(tree) == true);
    assert(tree->root->keys[0] == 15 && tree->root->children[0]->keys[0] == 10 && tree->root->children[1]->keys[0] == 20);
    // print_btree(tree);
    destroy_btree(tree);

    // Case: Borrow from left sibling (leaf)
    tree = create_btree(2);
    // Setup: Root[15], L[5,10], R[20]
    insert_btree(tree, 15); insert_btree(tree, 5); insert_btree(tree, 10); insert_btree(tree, 20);
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);
    delete_btree(tree, 20); // R child [20] underflows, borrows from L[5,10] via parent [15]
                            // Parent becomes [10], L[5], R[15]
    assert(search_btree(tree, 20) == false);
    assert(check_btree_properties(tree) == true);
    assert(tree->root->keys[0] == 10 && tree->root->children[0]->keys[0] == 5 && tree->root->children[1]->keys[0] == 15);
    // print_btree(tree);
    destroy_btree(tree);

    // Case: Merge with right sibling (children are leaves)
    tree = create_btree(2);
    // Setup: Root[10], L[5], R[15]. Both children have t-1 keys.
    insert_btree(tree, 10); insert_btree(tree, 5); insert_btree(tree, 15);
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);
    delete_btree(tree, 5); // L child [5] underflows, cannot borrow, merges with R[15].
                           // Parent [10] moves down. New root is [10,15] (if root was [10])
                           // Actually, parent [10] becomes empty, its child [5(key from parent),15] becomes root.
                           // So root becomes [10,15] (after merge, original root [10] is empty, child [5,10,15] becomes root)
                           // Wait, key from parent (10) moves to L. L becomes [5,10,15]. R is empty.
                           // Parent becomes empty. Root becomes [5,10,15].
    assert(search_btree(tree, 5) == false);
    assert(check_btree_properties(tree) == true);
    assert(tree->root->num_keys == 2 && tree->root->keys[0] == 10 && tree->root->keys[1] == 15);
    // print_btree(tree);
    destroy_btree(tree);

    // Case: Deletion from internal node, replace with predecessor, then handle underflow in leaf
    tree = create_btree(2);
    // Build: 10, 20, 30, 5, 15, 25, 35, 12, 17 (creates some height)
    int vals[] = {10,20,30,5,15,25,35,12,17,1,8,13,18,22,28,32,38};
    for(int i=0; i<sizeof(vals)/sizeof(vals[0]); ++i) insert_btree(tree, vals[i]);
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);
    // Delete 20 (internal node). Predecessor is 18.
    // This will likely trigger cascading merges or borrows.
    delete_btree(tree, 20);
    assert(search_btree(tree, 20) == false);
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);
    destroy_btree(tree);

    // Case: Tree height shrinks
    tree = create_btree(2);
    insert_btree(tree, 10); insert_btree(tree, 20); // Root [10,20]
    insert_btree(tree, 5); // Root [5,10,20]
    insert_btree(tree, 15); // Split: Root[10], L[5], R[15,20]
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);
    delete_btree(tree, 5); // L[5] underflows, borrows from R[15,20]. Root[15], L[10], R[20]
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);
    delete_btree(tree, 10); // L[10] underflows, merges with R[20]. Root[15] becomes empty. New root [15,20]
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);
    delete_btree(tree, 20); // Root [15,20] becomes [15]
    assert(check_btree_properties(tree) == true);
    // print_btree(tree);
    delete_btree(tree, 15); // Root becomes empty leaf
    assert(check_btree_properties(tree) == true);
    assert(tree->root->num_keys == 0 && tree->root->is_leaf == true);
    // print_btree(tree);
    destroy_btree(tree);

    printf("  test_btree_delete_complex_merge_borrow: PASSED (relies on check_btree_properties)\n");
}

void test_btree_properties_after_ops(void) {
    printf("  Testing B-Tree properties after a mixed sequence of operations (Stress Test, t=3)...\n");
    BTree *tree = create_btree(3); // Use a slightly larger t
    const int num_ops = 300;
    const int max_val = 1000;
    int current_keys[num_ops] = {0}; // Crude way to track some existing keys for deletion
    int current_key_count = 0;

    for (int i = 0; i < num_ops; ++i) {
        int op_type = rand() % 3; // 0 for insert, 1 for delete, 2 for search
        int value = rand() % max_val;

        if (op_type == 0 || current_key_count < 20) { // Insert or if tree is small
            // printf("Op %d: Inserting %d\n", i, value);
            insert_btree(tree, value);
            bool found = false;
            for(int k=0; k<current_key_count; ++k) if(current_keys[k] == value) found=true;
            if(!found && current_key_count < num_ops) current_keys[current_key_count++] = value;

        } else if (op_type == 1 && current_key_count > 0) { // Delete
            int del_idx = rand() % current_key_count;
            value = current_keys[del_idx];
            // printf("Op %d: Deleting %d\n", i, value);
            delete_btree(tree, value);
            current_keys[del_idx] = current_keys[--current_key_count]; // Remove from our list
        } else { // Search
            // printf("Op %d: Searching %d\n", i, value);
            search_btree(tree, value); // Just call it, result not critical for property check
        }
        
        if (i % 20 == 0 || i == num_ops - 1) { // Check properties periodically
            // printf("  Stress Test: Current Size (approx from array): %d, BTree size: %d\n", current_key_count, tree->root->num_keys);
            // print_btree(tree);
            assert(check_btree_properties(tree) == true);
        }
    }
    // Final check
    assert(check_btree_properties(tree) == true);
    // printf("Final tree for stress test (t=3):\n");
    // print_btree(tree);

    destroy_btree(tree);
    printf("  test_btree_properties_after_ops: PASSED\n");
}
