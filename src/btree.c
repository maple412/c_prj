#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// --- Static Helper Function Prototypes ---

// Node creation and destruction
static BTreeNode* create_btree_node(int t, bool is_leaf);
static void destroy_btree_node_recursive(BTreeNode* node);

// Search helpers
static BTreeNode* search_recursive(BTreeNode* node, int key, int t_val); // Added t_val for consistency, though not strictly needed by search logic itself

// Insertion helpers
static void btree_split_child(BTreeNode* parent_node, int child_index, BTreeNode* child_to_split, int t);
static void btree_insert_nonfull(BTreeNode* node, int key, int t);

// Deletion helpers
static void btree_delete_recursive(BTreeNode* node, int key, int t);
static int find_key_index(BTreeNode* node, int key);
static void remove_key_from_node(BTreeNode* node, int key_index, int t); // Added t for context, might be useful
static void remove_child_from_node(BTreeNode* node, int child_index, int t); // Added t

static void fill_child_if_needed(BTreeNode* parent_node, int child_idx, int t);
static void borrow_from_prev_sibling(BTreeNode* deficient_node, BTreeNode* parent_node, int deficient_node_idx_in_parent, BTreeNode* left_sibling, int t);
static void borrow_from_next_sibling(BTreeNode* deficient_node, BTreeNode* parent_node, int deficient_node_idx_in_parent, BTreeNode* right_sibling, int t);
static void merge_children(BTreeNode* parent_node, int deficient_node_idx, int t); // Simplified signature, children derived from parent and idx
static int get_predecessor_key(BTreeNode* node, int t); // Gets largest key in subtree rooted at node
static int get_successor_key(BTreeNode* node, int t);   // Gets smallest key in subtree rooted at node

// Print helper
static void print_btree_recursive(BTreeNode* node, int level, int t);


// --- Lifecycle Functions ---

static BTreeNode* create_btree_node(int t, bool is_leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    if (node == NULL) {
        perror("Failed to allocate BTreeNode");
        return NULL;
    }
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    
    node->keys = (int*)malloc(sizeof(int) * (2 * t - 1)); // Max keys = 2t-1
    if (node->keys == NULL) {
        perror("Failed to allocate keys for BTreeNode");
        free(node);
        return NULL;
    }
    
    node->children = (BTreeNode**)malloc(sizeof(BTreeNode*) * (2 * t)); // Max children = 2t
    if (node->children == NULL) {
        perror("Failed to allocate children pointers for BTreeNode");
        free(node->keys);
        free(node);
        return NULL;
    }
    // Initialize children pointers to NULL (important for traversal and other ops)
    for (int i = 0; i < (2 * t); i++) {
        node->children[i] = NULL;
    }
    return node;
}

BTree* create_btree(int t) {
    if (t < 2) {
        fprintf(stderr, "Error: B-Tree minimum degree 't' must be at least 2.\n");
        return NULL;
    }
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    if (tree == NULL) {
        perror("Failed to allocate memory for BTree");
        return NULL;
    }
    tree->t = t;
    tree->root = create_btree_node(t, true); // Root is initially a leaf
    if (tree->root == NULL) { // Check if node creation failed
        free(tree);
        return NULL;
    }
    return tree;
}

static void destroy_btree_node_recursive(BTreeNode* node) {
    if (node == NULL) {
        return;
    }
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) { // Iterate through all possible children
            destroy_btree_node_recursive(node->children[i]);
        }
    }
    free(node->keys);
    free(node->children);
    free(node);
}

void destroy_btree(BTree* tree) {
    if (tree == NULL) {
        return;
    }
    if (tree->root != NULL) {
        destroy_btree_node_recursive(tree->root);
    }
    free(tree);
}

// --- Search Operations ---

static BTreeNode* search_recursive(BTreeNode* node, int key, int t_val) { // t_val not used here but kept for possible future uses or consistency
    if (node == NULL) {
        return NULL;
    }
    int i = 0;
    // Find the first key greater than or equal to key
    while (i < node->num_keys && key > node->keys[i]) {
        i++;
    }

    // If key is found at index i in this node
    if (i < node->num_keys && key == node->keys[i]) {
        return node; // Key found in this node
    }

    // If this is a leaf node and key is not found
    if (node->is_leaf) {
        return NULL; // Key not found
    }

    // Go to the appropriate child
    return search_recursive(node->children[i], key, t_val);
}

bool search_btree(BTree* tree, int key) {
    if (tree == NULL || tree->root == NULL) {
        return false;
    }
    return search_recursive(tree->root, key, tree->t) != NULL;
}


// --- Insertion Operations ---

// Split the child y (child_to_split) of this node (parent_node).
// y must be full when this function is called.
// child_index is the index of y in parent_node's children array.
static void btree_split_child(BTreeNode* parent_node, int child_index, BTreeNode* full_child_node, int t) {
    BTreeNode* new_sibling_node = create_btree_node(t, full_child_node->is_leaf);
    if (new_sibling_node == NULL) {
        fprintf(stderr, "Error: Failed to create node for split.\n");
        // This is a critical error, might need better handling or program termination
        return;
    }
    new_sibling_node->num_keys = t - 1; // New node gets t-1 keys

    // Copy the last (t-1) keys from full_child_node to new_sibling_node
    for (int j = 0; j < t - 1; j++) {
        new_sibling_node->keys[j] = full_child_node->keys[j + t]; // Keys from t to 2t-2
    }

    // If full_child_node is not a leaf, copy its last t children to new_sibling_node
    if (!full_child_node->is_leaf) {
        for (int j = 0; j < t; j++) {
            new_sibling_node->children[j] = full_child_node->children[j + t]; // Children from t to 2t-1
        }
    }

    full_child_node->num_keys = t - 1; // Reduce the number of keys in full_child_node (median key at t-1 is moved up)

    // Make space for the new child in parent_node
    for (int j = parent_node->num_keys; j >= child_index + 1; j--) {
        parent_node->children[j + 1] = parent_node->children[j];
    }
    parent_node->children[child_index + 1] = new_sibling_node;

    // Make space for the median key from full_child_node in parent_node
    for (int j = parent_node->num_keys - 1; j >= child_index; j--) {
        parent_node->keys[j + 1] = parent_node->keys[j];
    }
    parent_node->keys[child_index] = full_child_node->keys[t - 1]; // Promote median key
    parent_node->num_keys++;
}

static void btree_insert_nonfull(BTreeNode* node, int key, int t) {
    int i = node->num_keys - 1; // Start from the rightmost key

    if (node->is_leaf) {
        // Find location for new key and shift greater keys to the right
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else { // Internal node
        // Find the child which is going to have the new key
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++; // Child index to descend into

        // If the found child is full, split it
        if (node->children[i]->num_keys == (2 * t - 1)) {
            btree_split_child(node, i, node->children[i], t);
            // After split, the key to be inserted might go into the new sibling
            if (key > node->keys[i]) {
                i++; // Key goes into the new sibling (which is now at index i)
            }
        }
        btree_insert_nonfull(node->children[i], key, t);
    }
}

void insert_btree(BTree* tree, int key) {
    if (tree == NULL) {
        fprintf(stderr, "Error: Cannot insert into a NULL B-Tree.\n");
        return;
    }
    BTreeNode* root_node = tree->root;

    if (root_node->num_keys == (2 * tree->t - 1)) { // If root is full
        BTreeNode* new_root = create_btree_node(tree->t, false); // New root is not a leaf
        if (new_root == NULL) {
            fprintf(stderr, "Error: Failed to create new root during split for insertion.\n");
            return;
        }
        tree->root = new_root;
        new_root->children[0] = root_node; // Old root becomes the first child of the new root
        
        btree_split_child(new_root, 0, root_node, tree->t);
        btree_insert_nonfull(new_root, key, tree->t);
    } else {
        btree_insert_nonfull(root_node, key, tree->t);
    }
}

// --- Deletion Operations (Placeholders for now) ---

static int find_key_index(BTreeNode* node, int key) {
    int idx = 0;
    // Find the index of the first key that is greater than or equal to k
    while (idx < node->num_keys && node->keys[idx] < key) {
        idx++;
    }
    return idx;
}

// Removes key at key_index from node. Assumes node is not empty.
static void remove_key_from_node(BTreeNode* node, int key_index, int t) {
    if (key_index < 0 || key_index >= node->num_keys) {
        fprintf(stderr, "Error: Invalid key_index for removal.\n");
        return;
    }
    // Shift keys after key_index to the left
    for (int i = key_index; i < node->num_keys - 1; i++) {
        node->keys[i] = node->keys[i + 1];
    }
    node->num_keys--;
}

// Removes child pointer at child_index from node.
static void remove_child_from_node(BTreeNode* node, int child_index, int t) {
     if (child_index < 0 || child_index > node->num_keys) { // Can remove child at num_keys index (after last key)
        fprintf(stderr, "Error: Invalid child_index for removal.\n");
        return;
    }
    // Shift children after child_index to the left
    for (int i = child_index; i < node->num_keys; i++) { // Iterate up to num_keys as children are num_keys+1
        node->children[i] = node->children[i + 1];
    }
    // Null out the last child pointer that was shifted
    node->children[node->num_keys] = NULL; // num_keys is already decremented if a key was removed that led to this child removal (e.g. merge)
                                         // This function itself does not directly change num_keys.
                                         // The number of children is num_keys + 1. If a key from parent is merged down, num_keys of parent decreases.
                                         // So this loop should go up to new_num_keys_of_parent.
                                         // This is tricky. Let's assume this is called after parent's key is removed.
}


static int get_predecessor_key(BTreeNode* node, int t) { // node is the child left of the key to be replaced
    // The predecessor is the rightmost key in the subtree rooted at 'node'.
    while (!node->is_leaf) {
        node = node->children[node->num_keys]; // Go to the rightmost child
    }
    return node->keys[node->num_keys - 1]; // Return the rightmost key of the leaf
}

static int get_successor_key(BTreeNode* node, int t) { // node is the child right of the key to be replaced
    // The successor is the leftmost key in the subtree rooted at 'node'.
    while (!node->is_leaf) {
        node = node->children[0]; // Go to the leftmost child
    }
    return node->keys[0]; // Return the leftmost key of the leaf
}


static void borrow_from_prev_sibling(BTreeNode* deficient_node, BTreeNode* parent_node, int deficient_node_idx_in_parent, BTreeNode* left_sibling, int t) {
    // Shift all keys and children in deficient_node one step to the right
    for (int i = deficient_node->num_keys - 1; i >= 0; i--) {
        deficient_node->keys[i + 1] = deficient_node->keys[i];
    }
    if (!deficient_node->is_leaf) {
        for (int i = deficient_node->num_keys; i >= 0; i--) {
            deficient_node->children[i + 1] = deficient_node->children[i];
        }
    }

    // Move parent's key down to deficient_node
    deficient_node->keys[0] = parent_node->keys[deficient_node_idx_in_parent - 1];
    // Move left_sibling's last key up to parent
    parent_node->keys[deficient_node_idx_in_parent - 1] = left_sibling->keys[left_sibling->num_keys - 1];

    // If left_sibling is not a leaf, move its last child to deficient_node's first child
    if (!left_sibling->is_leaf) {
        deficient_node->children[0] = left_sibling->children[left_sibling->num_keys];
        // left_sibling->children[left_sibling->num_keys] = NULL; // Not strictly necessary if num_keys is decremented
    }

    deficient_node->num_keys++;
    left_sibling->num_keys--;
}

static void borrow_from_next_sibling(BTreeNode* deficient_node, BTreeNode* parent_node, int deficient_node_idx_in_parent, BTreeNode* right_sibling, int t) {
    // Move parent's key down to deficient_node's last key position
    deficient_node->keys[deficient_node->num_keys] = parent_node->keys[deficient_node_idx_in_parent];
    // Move right_sibling's first key up to parent
    parent_node->keys[deficient_node_idx_in_parent] = right_sibling->keys[0];

    // If right_sibling is not a leaf, move its first child to deficient_node's last child position
    if (!right_sibling->is_leaf) {
        deficient_node->children[deficient_node->num_keys + 1] = right_sibling->children[0];
    }

    // Shift keys and children in right_sibling one step to the left
    for (int i = 1; i < right_sibling->num_keys; i++) {
        right_sibling->keys[i - 1] = right_sibling->keys[i];
    }
    if (!right_sibling->is_leaf) {
        for (int i = 1; i <= right_sibling->num_keys; i++) {
            right_sibling->children[i - 1] = right_sibling->children[i];
        }
        // right_sibling->children[right_sibling->num_keys] = NULL; // Not strictly necessary
    }
    
    deficient_node->num_keys++;
    right_sibling->num_keys--;
}

// Merges right_child (child at child_idx in parent) with left_child (child at child_idx-1 in parent).
// Key from parent at parent_key_idx (which is child_idx-1) is moved down.
// The merged content goes into left_child. right_child is freed.
static void merge_children(BTreeNode* parent_node, int parent_key_idx_to_move_down, int t) {
    BTreeNode* left_child = parent_node->children[parent_key_idx_to_move_down];
    BTreeNode* right_child = parent_node->children[parent_key_idx_to_move_down + 1];

    // Move key from parent down to the end of left_child's keys
    left_child->keys[left_child->num_keys] = parent_node->keys[parent_key_idx_to_move_down];
    left_child->num_keys++;

    // Copy keys from right_child to left_child
    for (int i = 0; i < right_child->num_keys; i++) {
        left_child->keys[left_child->num_keys] = right_child->keys[i];
        left_child->num_keys++;
    }

    // If children are not leaves, copy children pointers from right_child to left_child
    if (!left_child->is_leaf) { // or !right_child->is_leaf, they must be same type
        for (int i = 0; i <= right_child->num_keys; i++) {
            left_child->children[left_child->num_keys - right_child->num_keys + i -1] = right_child->children[i]; // Indexing needs care
        }
         // Corrected indexing for children copy:
        // left_child initially has 'lc_num_keys' keys and 'lc_num_keys+1' children.
        // It gets 1 key from parent, and 'rc_num_keys' keys from right_child.
        // Total keys in left_child after merge: lc_num_keys + 1 + rc_num_keys.
        // New number of children ptrs needed from right_child: rc_num_keys + 1.
        // These will be appended after left_child's original children.
        // The first key from parent is at left_child->keys[t-1] (if left_child had t-1 keys)
        // The children from right_child start at left_child->children[t] (if left_child had t-1 keys originally)
        int original_left_child_num_children = (left_child->num_keys - right_child->num_keys -1) +1; // num_children before adding from right sibling
         for (int i = 0; i <= right_child->num_keys; i++) {
            left_child->children[original_left_child_num_children + i] = right_child->children[i];
        }
    }
    
    // Remove key and child pointer from parent_node
    // Shift keys in parent_node
    for (int i = parent_key_idx_to_move_down; i < parent_node->num_keys - 1; i++) {
        parent_node->keys[i] = parent_node->keys[i + 1];
    }
    // Shift child pointers in parent_node
    for (int i = parent_key_idx_to_move_down + 1; i < parent_node->num_keys; i++) { // iterate up to current num_keys
        parent_node->children[i] = parent_node->children[i + 1];
    }
    parent_node->num_keys--;
    // Null out the last child pointer that was shifted if it's no longer valid.
    // parent_node->children[parent_node->num_keys +1] = NULL; // This is now beyond the new valid range of children

    // Free the right_child node
    free(right_child->keys);
    free(right_child->children);
    free(right_child);
}


// Ensures that child at child_idx in parent_node has at least t keys.
static void fill_child_if_needed(BTreeNode* parent_node, int child_idx, int t) {
    BTreeNode* child_to_fill = parent_node->children[child_idx];

    if (child_to_fill->num_keys < t -1 ) { // Should be t-1, not t. Min keys is t-1.
         fprintf(stderr, "Error: fill_child_if_needed called on child with %d keys (t-1=%d), should only be called if child has t-1 keys.\n", child_to_fill->num_keys, t-1);
         return; // Or handle error
    }
    if (child_to_fill->num_keys == t - 1) {
        // Try to borrow from left sibling
        if (child_idx > 0 && parent_node->children[child_idx - 1]->num_keys >= t) {
            borrow_from_prev_sibling(child_to_fill, parent_node, child_idx, parent_node->children[child_idx - 1], t);
        }
        // Try to borrow from right sibling
        else if (child_idx < parent_node->num_keys && parent_node->children[child_idx + 1]->num_keys >= t) {
            borrow_from_next_sibling(child_to_fill, parent_node, child_idx, parent_node->children[child_idx + 1], t);
        }
        // Merge if borrowing failed
        else {
            if (child_idx < parent_node->num_keys) { // Merge with right sibling
                merge_children(parent_node, child_idx, t); // Key at child_idx moves down. Merged into child_idx.
            } else { // Merge with left sibling (child_idx is the rightmost child)
                merge_children(parent_node, child_idx - 1, t); // Key at child_idx-1 moves down. Merged into child_idx-1.
            }
        }
    }
}


static void btree_delete_recursive(BTreeNode* node, int key, int t) {
    if (node == NULL) return;

    int idx = find_key_index(node, key);

    // Case 1: Key is in current node
    if (idx < node->num_keys && node->keys[idx] == key) {
        if (node->is_leaf) { // Key in leaf node
            remove_key_from_node(node, idx, t);
        } else { // Key in internal node
            BTreeNode* left_child_of_key = node->children[idx];
            BTreeNode* right_child_of_key = node->children[idx + 1];

            if (left_child_of_key->num_keys >= t) { // If left child has enough keys, find predecessor
                int predecessor = get_predecessor_key(left_child_of_key, t);
                node->keys[idx] = predecessor; // Replace key with predecessor
                btree_delete_recursive(left_child_of_key, predecessor, t); // Recursively delete predecessor
            } else if (right_child_of_key->num_keys >= t) { // If right child has enough keys, find successor
                int successor = get_successor_key(right_child_of_key, t);
                node->keys[idx] = successor; // Replace key with successor
                btree_delete_recursive(right_child_of_key, successor, t); // Recursively delete successor
            } else { // Both children have t-1 keys, merge them
                // The key from node (node->keys[idx]) will be moved down into left_child_of_key
                // right_child_of_key's contents are moved into left_child_of_key
                // right_child_of_key is then freed.
                // The key to be deleted (original 'key') is now in the merged child (left_child_of_key).
                int key_to_delete_from_parent = node->keys[idx]; // Save before it might be overwritten by merge logic
                
                // Merge right_child_of_key into left_child_of_key.
                // The key from parent (node->keys[idx]) is part of this merge.
                // The child_index for merge is 'idx' (key at idx in parent is moved down).
                merge_children(node, idx, t); 
                
                // After merge, node->children[idx] is the merged node.
                // The original key 'key' (which was node->keys[idx] before merge logic shifted it)
                // is now inside node->children[idx]. So delete it from there.
                btree_delete_recursive(node->children[idx], key, t);
            }
        }
    } else { // Case 2: Key is not in current node, so it must be in a child
        if (node->is_leaf) {
            // Key not found and this is a leaf.
            // printf("Key %d not found in tree.\n", key);
            return;
        }

        BTreeNode* child_to_descend = node->children[idx];
        bool last_child_chosen = (idx == node->num_keys); // True if key > all keys in node, so descend into last child

        if (child_to_descend->num_keys < t) { // If child has t-1 keys (minimum)
             // fill_child_if_needed will ensure child_to_descend has at least t keys.
             // It might change parent node's structure or which child contains the key.
             // Specifically, if merge_children was called, the child_idx might need adjustment.
            fill_child_if_needed(node, idx, t);
            
            // If fill_child_if_needed caused a merge with the left sibling (idx was merged into idx-1),
            // and the key we are looking for was greater than the key that moved down from parent,
            // then we should now descend into the child at idx-1 instead of idx.
            // This happens if idx > parent_key_idx_that_moved_down.
            // The current fill_child_if_needed logic:
            // if child_idx < parent_node->num_keys (merge with right, new node is at child_idx)
            // else (merge with left, new node is at child_idx-1)
            // If we merged with left (child_idx was parent_node->num_keys), then the new child to descend into is at child_idx-1.
            // And if the key was in the original child_idx (which was merged), it's now in child_idx-1.
            // The find_key_index(node, key) would give the same 'idx' before and after merge if parent keys changed.
            // Re-evaluate idx based on current node state if keys in parent changed.
            // This part is tricky. The CLRS approach often re-finds the path or assumes fill ensures current child is okay.
            // A simpler way for now: if a merge occurred, the key might be in a different child.
            // The current `fill_child_if_needed` tries to make `node->children[idx]` safe.
            // If `node->children[idx]` was merged into `node->children[idx-1]`, then `idx` itself is no longer valid in the same way.
            // Let's assume `fill_child_if_needed` ensures `node->children[idx]` is valid to descend or `idx` is adjusted.
            // The current `fill_child_if_needed` might merge child `idx` with `idx+1` (into `idx`) or with `idx-1` (into `idx-1`).
            // If child `idx` was merged with `idx-1` (meaning `idx` was the right child of the merge), then we need to descend into `idx-1`.
            // This happens if `idx == node->num_keys` *after* the key from parent has been moved down.
            // The current `idx` from `find_key_index` is where key *would* be or where path *should* go.
            // If `fill_child_if_needed` merged `node->children[idx]` with `node->children[idx-1]`, then `idx` should be decremented.
            // This occurs if `idx > 0` and `node->children[idx]` was merged into `node->children[idx-1]`.
            // The `fill_child_if_needed` calls merge with `child_idx` or `child_idx-1` as the target of merge.
            // If merged into `child_idx-1`, we should descend into `child_idx-1`.
            // This logic needs to be solid in `fill_child_if_needed` or handled here.
            // For now, let's assume `fill_child_if_needed` makes `node->children[idx]` have >= t keys,
            // or if it was merged away, then the key must now be in the child that it was merged into.
            // Let's refine `idx` after `fill_child_if_needed`:
            // If the original child `node->children[idx]` was merged into `node->children[idx-1]`,
            // this means `parent_node->keys[idx-1]` was pulled down.
            // And if our `key > parent_node->keys[idx-1]` (original value before pull-down), we'd still go to the right of that.
            // This part of deletion is the most complex.
            // A common strategy is that fill_child_if_needed guarantees that node->children[idx] has at least t keys.
            // If node->children[idx] was merged with node->children[idx-1] (idx was the right part of merge)
            // then the effective child index to descend into becomes idx-1.
            // The current fill_child_if_needed merges into child_idx (if merging with right) or child_idx-1 (if merging with left).
            // If it merged child_idx into child_idx-1, then idx must be decremented.
            // This happens if child_idx was the rightmost child and merged with its left sibling.
            if (idx > node->num_keys && idx > 0) { // If key was > all keys in node, and a merge with left happened, idx should point to new merged node
                idx--;                           // (idx was originally node->num_keys+1 effectively, now node->num_keys after merge)
            }

        }
         btree_delete_recursive(node->children[idx], key, t);
    }
}


void delete_btree(BTree* tree, int key) {
    if (tree == NULL || tree->root == NULL || (tree->root->num_keys == 0 && tree->root->is_leaf)) {
        // Tree is empty or key cannot exist
        return;
    }
    btree_delete_recursive(tree->root, key, tree->t);

    // If the root node has 0 keys after deletion and it's not a leaf,
    // make its only child (children[0]) the new root.
    if (tree->root->num_keys == 0 && !tree->root->is_leaf) {
        BTreeNode* old_root = tree->root;
        if (old_root->children[0] == NULL) {
             // This case implies an issue, an internal node with 0 keys should have one child if tree is not empty.
             // Or, if this child is also empty (num_keys=0, is_leaf=true), then tree becomes empty.
             // For simplicity, if old_root->children[0] is NULL, it means tree is effectively empty.
             // Let's assume children[0] is valid if num_keys=0 and not is_leaf.
             // This state means the tree became empty through deletion.
             // A more robust check: if old_root->children[0] is a leaf with 0 keys, then tree is empty.
             // The create_btree ensures root is a leaf. If all keys are deleted, root remains leaf with 0 keys.
             // This condition (num_keys == 0 && !is_leaf) means root *had* children.
             // So, children[0] should be the new root.
        }
        tree->root = old_root->children[0];
        // Free old_root's key and children arrays, then old_root itself
        free(old_root->keys);
        free(old_root->children);
        free(old_root);
        if (tree->root == NULL) { // If the child was NULL (should not happen for non-leaf with 0 keys unless tree becomes empty)
            tree->root = create_btree_node(tree->t, true); // Make tree empty but valid
             if (!tree->root) {fprintf(stderr, "Failed to re-initialize root after deletion\n");}
        }
    }
     // If tree->root is NULL after all deletions (e.g. last key in root was deleted, and root was a leaf)
    // then make a new empty root.
    if (tree->root->num_keys == 0 && tree->root->is_leaf && tree->root != NULL /* to avoid double check if it became NULL above */) {
        // Tree is now empty. Root is a leaf with 0 keys. This is fine.
    }


}


// --- Utility / Debugging ---
static void print_btree_recursive(BTreeNode* node, int level, int t) {
    if (node == NULL) return;
    
    // Indentation for current level
    for(int k=0; k<level; ++k) printf("  ");

    printf("L%d: [", level);
    for (int i = 0; i < node->num_keys; i++) {
        printf("%d", node->keys[i]);
        if (i < node->num_keys - 1) printf(", ");
    }
    printf("] (Leaf: %s, Keys: %d)", node->is_leaf ? "Y" : "N", node->num_keys);
    // For debugging, print number of children if internal node
    if (!node->is_leaf) {
        int child_count = 0;
        for(int i=0; i<=node->num_keys; ++i) if(node->children[i]) child_count++;
        printf(" (Children: %d)\n", child_count);
    } else {
        printf("\n");
    }


    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) { // Iterate through all children
            if (node->children[i] != NULL) {
                 print_btree_recursive(node->children[i], level + 1, t);
            } else {
                // Optionally print placeholder for NULL children if needed for debugging structure
                // for(int k=0; k<level+1; ++k) printf("  ");
                // printf("L%d Child %d: NULL\n", level+1, i);
            }
        }
    }
}

void print_btree(BTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("B-Tree is empty or NULL.\n");
        return;
    }
    printf("B-Tree Structure (t=%d):\n", tree->t);
    print_btree_recursive(tree->root, 0, tree->t);
    printf("------------------------\n");
}
