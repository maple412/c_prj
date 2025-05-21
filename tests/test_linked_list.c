#include <stdio.h>
#include <stdlib.h>
#include <assert.h> // For basic assertions

// Include the linked list header file from the src directory
#include "../src/linked_list.h"

// Test function prototypes
void test_create_destroy_list(void);
void test_append_and_get_node(void);
void test_prepend_node(void);
void test_insert_node_at(void);
void test_remove_node_at(void);
void test_search_node(void);
void test_get_list_size(void);
void test_print_linked_list(void);
void test_edge_cases_and_null_handling(void);


// Main function to run all tests
int main() {
    printf("Running linked list tests...\n\n");

    test_create_destroy_list();
    test_append_and_get_node();
    test_prepend_node();
    test_insert_node_at();
    test_remove_node_at();
    test_search_node();
    test_get_list_size();
    test_print_linked_list();
    test_edge_cases_and_null_handling();

    printf("\nAll linked list tests completed successfully!\n");
    return 0;
}

// Implementations of test functions

void test_create_destroy_list(void) {
    printf("  Testing list creation and destruction...\n");
    LinkedList *list = create_linked_list();
    assert(list != NULL);
    assert(list->head == NULL);
    assert(list->size == 0);
    assert(get_list_size(list) == 0);

    // Populate then destroy to check if nodes are freed (implicitly)
    append_node(list, 10);
    append_node(list, 20);
    assert(list->size == 2);
    destroy_linked_list(list); 
    // If destroy_linked_list is correct, valgrind would show no leaks.
    // For now, we ensure it doesn't crash and can be called again.

    // Test destroying a NULL list (should not crash)
    destroy_linked_list(NULL);

    // Test destroying an already destroyed list (or an empty one created anew)
    LinkedList *list2 = create_linked_list();
    destroy_linked_list(list2);
    destroy_linked_list(list2); // Call again

    printf("  test_create_destroy_list: PASSED\n");
}

void test_append_and_get_node(void) {
    printf("  Testing append_node and get_node_at...\n");
    LinkedList *list = create_linked_list();
    assert(list != NULL);
    Node* node_ptr;

    // Append first element
    assert(append_node(list, 10) == 0);
    assert(list->size == 1);
    assert(get_list_size(list) == 1);
    node_ptr = get_node_at(list, 0);
    assert(node_ptr != NULL && node_ptr->data == 10);

    // Append second element
    assert(append_node(list, 20) == 0);
    assert(list->size == 2);
    assert(get_list_size(list) == 2);
    node_ptr = get_node_at(list, 1);
    assert(node_ptr != NULL && node_ptr->data == 20);
    // Check first element again
    node_ptr = get_node_at(list, 0);
    assert(node_ptr != NULL && node_ptr->data == 10);


    // Append third element
    assert(append_node(list, 30) == 0);
    assert(list->size == 3);
    node_ptr = get_node_at(list, 2);
    assert(node_ptr != NULL && node_ptr->data == 30);

    // Test get_node_at with out-of-bounds indices
    assert(get_node_at(list, list->size) == NULL); // Index == size
    assert(get_node_at(list, 100) == NULL);    // Large index
    
    // Test get_node_at on an empty list
    LinkedList* empty_list = create_linked_list();
    assert(get_node_at(empty_list, 0) == NULL);
    destroy_linked_list(empty_list);

    destroy_linked_list(list);
    printf("  test_append_and_get_node: PASSED\n");
}

void test_prepend_node(void) {
    printf("  Testing prepend_node...\n");
    LinkedList *list = create_linked_list();
    assert(list != NULL);
    Node* node_ptr;

    // Prepend first element
    assert(prepend_node(list, 10) == 0); // List: [10]
    assert(list->size == 1);
    assert(get_list_size(list) == 1);
    assert(list->head != NULL && list->head->data == 10);
    node_ptr = get_node_at(list, 0);
    assert(node_ptr != NULL && node_ptr->data == 10);

    // Prepend second element
    assert(prepend_node(list, 20) == 0); // List: [20, 10]
    assert(list->size == 2);
    assert(get_list_size(list) == 2);
    assert(list->head != NULL && list->head->data == 20);
    node_ptr = get_node_at(list, 0);
    assert(node_ptr != NULL && node_ptr->data == 20);
    node_ptr = get_node_at(list, 1);
    assert(node_ptr != NULL && node_ptr->data == 10);

    // Prepend third element
    assert(prepend_node(list, 30) == 0); // List: [30, 20, 10]
    assert(list->size == 3);
    assert(list->head != NULL && list->head->data == 30);
    node_ptr = get_node_at(list, 0);
    assert(node_ptr != NULL && node_ptr->data == 30);
    node_ptr = get_node_at(list, 1);
    assert(node_ptr != NULL && node_ptr->data == 20);
    node_ptr = get_node_at(list, 2);
    assert(node_ptr != NULL && node_ptr->data == 10);

    destroy_linked_list(list);
    printf("  test_prepend_node: PASSED\n");
}

void test_insert_node_at(void) {
    printf("  Testing insert_node_at...\n");
    LinkedList *list = create_linked_list();
    assert(list != NULL);
    Node* node_ptr;

    // Insert into an empty list at index 0
    assert(insert_node_at(list, 10, 0) == 0); // List: [10]
    assert(list->size == 1);
    assert(get_list_size(list) == 1);
    node_ptr = get_node_at(list, 0);
    assert(node_ptr != NULL && node_ptr->data == 10);

    // Insert at the beginning (index 0) of non-empty list
    assert(insert_node_at(list, 5, 0) == 0);  // List: [5, 10]
    assert(list->size == 2);
    node_ptr = get_node_at(list, 0);
    assert(node_ptr != NULL && node_ptr->data == 5);
    node_ptr = get_node_at(list, 1);
    assert(node_ptr != NULL && node_ptr->data == 10);

    // Insert at the end (index list->size)
    assert(insert_node_at(list, 20, list->size) == 0); // List: [5, 10, 20] (size was 2)
    assert(list->size == 3);
    node_ptr = get_node_at(list, 2);
    assert(node_ptr != NULL && node_ptr->data == 20);
    // Check previous last node
    node_ptr = get_node_at(list, 1);
    assert(node_ptr != NULL && node_ptr->data == 10);


    // Insert in the middle
    assert(insert_node_at(list, 15, 2) == 0); // List: [5, 10, 15, 20] (index 2, new size will be 4)
    assert(list->size == 4);
    node_ptr = get_node_at(list, 1); // before insert point
    assert(node_ptr != NULL && node_ptr->data == 10);
    node_ptr = get_node_at(list, 2); // new node
    assert(node_ptr != NULL && node_ptr->data == 15);
    node_ptr = get_node_at(list, 3); // after insert point
    assert(node_ptr != NULL && node_ptr->data == 20);


    // Test insertion at invalid index (index > size)
    assert(insert_node_at(list, 999, list->size + 1) == -1);
    assert(insert_node_at(list, 999, 100) == -1); // Way out of bounds
    assert(list->size == 4); // Size should be unchanged

    destroy_linked_list(list);
    printf("  test_insert_node_at: PASSED\n");
}

void test_remove_node_at(void) {
    printf("  Testing remove_node_at...\n");
    LinkedList *list = create_linked_list();
    assert(list != NULL);
    Node* node_ptr;

    // Populate list: [0, 10, 20, 30, 40]
    for (int i = 0; i < 5; ++i) {
        append_node(list, i * 10);
    }
    assert(get_list_size(list) == 5);

    // Remove from middle
    assert(remove_node_at(list, 2) == 0); // Remove 20 -> [0, 10, 30, 40]
    assert(get_list_size(list) == 4);
    node_ptr = get_node_at(list, 1);
    assert(node_ptr != NULL && node_ptr->data == 10);
    node_ptr = get_node_at(list, 2);
    assert(node_ptr != NULL && node_ptr->data == 30);

    // Remove from beginning
    assert(remove_node_at(list, 0) == 0); // Remove 0 -> [10, 30, 40]
    assert(get_list_size(list) == 3);
    node_ptr = get_node_at(list, 0);
    assert(node_ptr != NULL && node_ptr->data == 10);
    node_ptr = get_node_at(list, 1);
    assert(node_ptr != NULL && node_ptr->data == 30);

    // Remove from end
    assert(remove_node_at(list, get_list_size(list) - 1) == 0); // Remove 40 -> [10, 30]
    assert(get_list_size(list) == 2);
    node_ptr = get_node_at(list, get_list_size(list) - 1); // new last element
    assert(node_ptr != NULL && node_ptr->data == 30);
    node_ptr = get_node_at(list, 0); // first element
    assert(node_ptr != NULL && node_ptr->data == 10);


    // Remove remaining elements to make it empty
    assert(remove_node_at(list, 0) == 0); // Remove 10 -> [30]
    assert(get_list_size(list) == 1);
    assert(remove_node_at(list, 0) == 0); // Remove 30 -> []
    assert(get_list_size(list) == 0);
    assert(list->head == NULL);

    // Test removal from empty list
    assert(remove_node_at(list, 0) == -1);

    // Test removal at invalid index
    append_node(list, 1); append_node(list, 2); // List: [1, 2]
    assert(remove_node_at(list, 2) == -1); // Index == size
    assert(remove_node_at(list, 100) == -1);
    assert(get_list_size(list) == 2); // Size unchanged

    destroy_linked_list(list);
    printf("  test_remove_node_at: PASSED\n");
}

void test_search_node(void) {
    printf("  Testing search_node...\n");
    LinkedList *list = create_linked_list();
    assert(list != NULL);

    // Populate: [10, 20, 30, 40, 50]
    for (int i = 0; i < 5; ++i) {
        append_node(list, (i + 1) * 10);
    }

    // Search for existing elements
    assert(search_node(list, 10) == 0);  // Beginning
    assert(search_node(list, 30) == 2);  // Middle
    assert(search_node(list, 50) == 4);  // End

    // Search for non-existent element
    assert(search_node(list, 99) == -1);

    // Search in an empty list
    LinkedList *empty_list = create_linked_list();
    assert(search_node(empty_list, 10) == -1);
    destroy_linked_list(empty_list);
    
    // Search for an element that appears multiple times (should return first occurrence)
    append_node(list, 30); // list is now [10, 20, 30, 40, 50, 30]
    assert(get_list_size(list) == 6);
    assert(search_node(list, 30) == 2);

    destroy_linked_list(list);
    printf("  test_search_node: PASSED\n");
}

void test_get_list_size(void) {
    printf("  Testing get_list_size...\n");
    LinkedList *list = create_linked_list();
    assert(list != NULL);

    assert(get_list_size(list) == 0);
    assert(list->size == 0);

    // After append
    append_node(list, 10);
    assert(get_list_size(list) == 1);
    assert(list->size == 1);

    // After prepend
    prepend_node(list, 5); // [5, 10]
    assert(get_list_size(list) == 2);
    assert(list->size == 2);

    // After insert
    insert_node_at(list, 7, 1); // [5, 7, 10]
    assert(get_list_size(list) == 3);
    assert(list->size == 3);

    // After remove
    remove_node_at(list, 0); // [7, 10]
    assert(get_list_size(list) == 2);
    assert(list->size == 2);

    // Remove until empty
    remove_node_at(list, 1); // [7]
    assert(get_list_size(list) == 1);
    remove_node_at(list, 0); // []
    assert(get_list_size(list) == 0);

    destroy_linked_list(list);
    printf("  test_get_list_size: PASSED\n");
}

void test_print_linked_list(void) {
    printf("  Testing print_linked_list (manual verification)...\n");
    LinkedList *list = create_linked_list();
    
    printf("    Printing empty list:\n    ");
    print_linked_list(list); // Expect: List is empty. (Size: 0)

    append_node(list, 10);
    append_node(list, 20);
    append_node(list, 30);
    printf("    Printing list with 3 nodes:\n    ");
    print_linked_list(list); // Expect: List (Size: 3): [10 -> 20 -> 30]
    
    destroy_linked_list(list);

    printf("    Printing NULL list:\n    ");
    print_linked_list(NULL); // Expect: List is NULL.
    
    printf("  test_print_linked_list: (No automated assertions) PASSED\n");
}

void test_edge_cases_and_null_handling(void) {
    printf("  Testing edge cases and NULL list handling...\n");
    LinkedList *list_null = NULL;
    Node* node_ptr_null;

    // Test operations on NULL list
    assert(append_node(list_null, 1) == -1);
    assert(prepend_node(list_null, 1) == -1);
    assert(insert_node_at(list_null, 1, 0) == -1);
    assert(remove_node_at(list_null, 0) == -1);
    node_ptr_null = get_node_at(list_null, 0);
    assert(node_ptr_null == NULL);
    assert(search_node(list_null, 1) == -1);
    assert(get_list_size(list_null) == 0); // As per implementation
    print_linked_list(list_null); // Should print "List is NULL."

    // Test operations on an empty list (valid list pointer, but no nodes)
    LinkedList *empty_list = create_linked_list();
    assert(empty_list != NULL);
    assert(remove_node_at(empty_list, 0) == -1); // Remove from empty
    node_ptr_null = get_node_at(empty_list, 0);  // Get from empty
    assert(node_ptr_null == NULL);
    assert(search_node(empty_list, 1) == -1);   // Search in empty
    assert(get_list_size(empty_list) == 0);

    // Insert at index 0 in empty list (this is a valid operation)
    assert(insert_node_at(empty_list, 100, 0) == 0);
    assert(get_list_size(empty_list) == 1);
    node_ptr_null = get_node_at(empty_list, 0);
    assert(node_ptr_null != NULL && node_ptr_null->data == 100);

    destroy_linked_list(empty_list);

    printf("  test_edge_cases_and_null_handling: PASSED\n");
}
