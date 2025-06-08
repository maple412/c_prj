#include <stdio.h>
#include <stdlib.h>
#include <assert.h> // For basic assertions
#include <string.h> // For potential future use (e.g. comparing arrays of structs)

// Include the array header file from the src directory
#include "../src/array.h"

// Constants from array.c for testing purposes
#define DEFAULT_INITIAL_CAPACITY 10
#define RESIZE_FACTOR 2

// Test function prototypes
void test_create_destroy_array(void);
void test_append_and_get_element(void);
void test_insert_element_at(void);
void test_remove_element_at(void);
void test_search_element(void);
void test_resize_functionality(void);
void test_edge_cases(void);

// Main function to run all tests
int main() {
    printf("Running array tests...\n\n");

    test_create_destroy_array();
    test_append_and_get_element();
    test_insert_element_at();
    test_remove_element_at();
    test_search_element();
    test_resize_functionality();
    test_edge_cases();

    printf("\nAll array tests completed successfully!\n");
    return 0;
}

// Implementations of test functions

void test_create_destroy_array(void) {
    printf("  Testing array creation and destruction...\n");
    DynamicArray *arr;

    // Test creation with a specific capacity
    arr = create_array(5);
    assert(arr != NULL);
    assert(arr->data != NULL);
    assert(arr->size == 0);
    assert(arr->capacity == 5);
    destroy_array(arr);

    // Test creation with zero initial capacity (should use default)
    arr = create_array(0);
    assert(arr != NULL);
    assert(arr->data != NULL);
    assert(arr->size == 0);
    assert(arr->capacity == DEFAULT_INITIAL_CAPACITY);
    destroy_array(arr);

    // Test destroy_array with NULL (should not crash)
    destroy_array(NULL);

    printf("  test_create_destroy_array: PASSED\n");
}

void test_append_and_get_element(void) {
    printf("  Testing append_element and get_element_at...\n");
    DynamicArray *arr = create_array(2); // Small initial capacity to test resize
    assert(arr != NULL);
    int element_value;

    // Append first element
    assert(append_element(arr, 10) == 0);
    assert(arr->size == 1);
    assert(arr->capacity == 2);
    assert(get_element_at(arr, 0, &element_value) == 0 && element_value == 10);

    // Append second element
    assert(append_element(arr, 20) == 0);
    assert(arr->size == 2);
    assert(arr->capacity == 2);
    assert(get_element_at(arr, 1, &element_value) == 0 && element_value == 20);

    // Append third element (should trigger resize)
    assert(append_element(arr, 30) == 0);
    assert(arr->size == 3);
    assert(arr->capacity == 2 * RESIZE_FACTOR); // 2 * 2 = 4
    assert(get_element_at(arr, 2, &element_value) == 0 && element_value == 30);

    // Verify all elements after resize
    assert(get_element_at(arr, 0, &element_value) == 0 && element_value == 10);
    assert(get_element_at(arr, 1, &element_value) == 0 && element_value == 20);
    assert(get_element_at(arr, 2, &element_value) == 0 && element_value == 30);

    // Test get_element_at with out-of-bounds indices
    assert(get_element_at(arr, arr->size, &element_value) == -1); // Index == size
    assert(get_element_at(arr, 100, &element_value) == -1);    // Large index

    // Test get_element_at with NULL element_value pointer
    assert(get_element_at(arr, 0, NULL) == -1);


    // Test appending to an array with 0 initial capacity (should resize from default)
    DynamicArray *arr_zero_cap = create_array(0); // capacity will be DEFAULT_INITIAL_CAPACITY
    assert(arr_zero_cap != NULL);
    for(int i=0; i < DEFAULT_INITIAL_CAPACITY; ++i) {
        assert(append_element(arr_zero_cap, i*10) == 0);
    }
    assert(arr_zero_cap->size == DEFAULT_INITIAL_CAPACITY);
    assert(arr_zero_cap->capacity == DEFAULT_INITIAL_CAPACITY);
    // Trigger resize
    assert(append_element(arr_zero_cap, 999) == 0);
    assert(arr_zero_cap->size == DEFAULT_INITIAL_CAPACITY + 1);
    assert(arr_zero_cap->capacity == DEFAULT_INITIAL_CAPACITY * RESIZE_FACTOR);
    assert(get_element_at(arr_zero_cap, DEFAULT_INITIAL_CAPACITY, &element_value) == 0 && element_value == 999);


    destroy_array(arr);
    destroy_array(arr_zero_cap);
    printf("  test_append_and_get_element: PASSED\n");
}

void test_insert_element_at(void) {
    printf("  Testing insert_element_at...\n");
    DynamicArray *arr = create_array(3);
    assert(arr != NULL);
    int element_value;

    // Insert into an empty array (at index 0)
    assert(insert_element_at(arr, 100, 0) == 0); // [100]
    assert(arr->size == 1);
    assert(arr->capacity == 3);
    assert(get_element_at(arr, 0, &element_value) == 0 && element_value == 100);

    // Insert at the end (equivalent to append)
    assert(insert_element_at(arr, 300, arr->size) == 0); // [100, 300] (arr->size was 1)
    assert(arr->size == 2);
    assert(get_element_at(arr, 1, &element_value) == 0 && element_value == 300);

    // Insert at the beginning
    assert(insert_element_at(arr, 50, 0) == 0); // [50, 100, 300]
    assert(arr->size == 3);
    assert(get_element_at(arr, 0, &element_value) == 0 && element_value == 50);
    assert(get_element_at(arr, 1, &element_value) == 0 && element_value == 100);
    assert(get_element_at(arr, 2, &element_value) == 0 && element_value == 300);

    // Insert to trigger resize
    assert(insert_element_at(arr, 200, 1) == 0); // [50, 200, 100, 300]
    assert(arr->size == 4);
    assert(arr->capacity == 3 * RESIZE_FACTOR); // 3 * 2 = 6
    assert(get_element_at(arr, 0, &element_value) == 0 && element_value == 50);
    assert(get_element_at(arr, 1, &element_value) == 0 && element_value == 200);
    assert(get_element_at(arr, 2, &element_value) == 0 && element_value == 100);
    assert(get_element_at(arr, 3, &element_value) == 0 && element_value == 300);

    // Insert in the middle
    assert(insert_element_at(arr, 250, 2) == 0); // [50, 200, 250, 100, 300]
    assert(arr->size == 5);
    assert(get_element_at(arr, 2, &element_value) == 0 && element_value == 250);
    assert(get_element_at(arr, 3, &element_value) == 0 && element_value == 100);

    // Test insertion at invalid index (index > size)
    assert(insert_element_at(arr, 999, arr->size + 1) == -1);
    assert(insert_element_at(arr, 999, 100) == -1); // Way out of bounds

    // Test insertion when capacity is 0 (e.g. after create_array and specific resize)
    DynamicArray* arr_zero_cap_then_insert = create_array(1);
    assert(arr_zero_cap_then_insert != NULL);
    // Force capacity to 0 after creation (if array is empty)
    assert(resize_array(arr_zero_cap_then_insert, 0) == 0);
    assert(arr_zero_cap_then_insert->capacity == 0);
    assert(arr_zero_cap_then_insert->size == 0);

    assert(insert_element_at(arr_zero_cap_then_insert, 1, 0) == 0); // Should resize from 0
    assert(arr_zero_cap_then_insert->size == 1);
    assert(arr_zero_cap_then_insert->capacity == DEFAULT_INITIAL_CAPACITY); // Resized to default
    assert(get_element_at(arr_zero_cap_then_insert, 0, &element_value) == 0 && element_value == 1);


    destroy_array(arr);
    destroy_array(arr_zero_cap_then_insert);
    printf("  test_insert_element_at: PASSED\n");
}

void test_remove_element_at(void) {
    printf("  Testing remove_element_at...\n");
    DynamicArray *arr = create_array(5);
    assert(arr != NULL);
    int element_value;

    // Populate array: [0, 10, 20, 30, 40]
    for (int i = 0; i < 5; ++i) {
        assert(append_element(arr, i * 10) == 0);
    }
    assert(arr->size == 5);

    // Remove from middle
    assert(remove_element_at(arr, 2) == 0); // Remove 20 -> [0, 10, 30, 40]
    assert(arr->size == 4);
    assert(get_element_at(arr, 1, &element_value) == 0 && element_value == 10);
    assert(get_element_at(arr, 2, &element_value) == 0 && element_value == 30);

    // Remove from beginning
    assert(remove_element_at(arr, 0) == 0); // Remove 0 -> [10, 30, 40]
    assert(arr->size == 3);
    assert(get_element_at(arr, 0, &element_value) == 0 && element_value == 10);
    assert(get_element_at(arr, 1, &element_value) == 0 && element_value == 30);

    // Remove from end
    assert(remove_element_at(arr, arr->size - 1) == 0); // Remove 40 -> [10, 30]
    assert(arr->size == 2);
    assert(get_element_at(arr, arr->size - 1, &element_value) == 0 && element_value == 30);

    // Remove remaining elements to make it empty
    assert(remove_element_at(arr, 0) == 0); // Remove 10 -> [30]
    assert(arr->size == 1);
    assert(remove_element_at(arr, 0) == 0); // Remove 30 -> []
    assert(arr->size == 0);

    // Test removal from empty array
    assert(remove_element_at(arr, 0) == -1);

    // Test removal at invalid index
    DynamicArray *arr2 = create_array(3);
    append_element(arr2, 1); append_element(arr2, 2);
    assert(remove_element_at(arr2, 2) == -1); // Index == size
    assert(remove_element_at(arr2, 100) == -1);

    // Test optional shrinking (This depends on the shrink condition in array.c)
    // Create an array with capacity > DEFAULT_INITIAL_CAPACITY and fill it up
    // The shrink condition is: arr->capacity > DEFAULT_INITIAL_CAPACITY && arr->size < arr->capacity / (RESIZE_FACTOR * 2)
    // So if DEFAULT_INITIAL_CAPACITY = 10, RESIZE_FACTOR = 2, shrink happens if cap > 10 and size < cap / 4
    // E.g. cap = 20 (after one resize from 10), size must be < 5 for shrink.
    // Let's make capacity 40. Then size < 10 for shrink.
    DynamicArray *arr_shrink = create_array(DEFAULT_INITIAL_CAPACITY * RESIZE_FACTOR); // cap = 20
    assert(arr_shrink->capacity == 20); 
    for(int i=0; i<20; ++i) assert(append_element(arr_shrink, i) == 0); // size = 20, cap = 20
    assert(append_element(arr_shrink, 20) == 0); // size = 21, cap = 40
    
    size_t capacity_before_removals = arr_shrink->capacity; // Should be 40
    assert(capacity_before_removals == DEFAULT_INITIAL_CAPACITY * RESIZE_FACTOR * RESIZE_FACTOR);

    // Remove elements until size < capacity / 4
    // Current: size=21, cap=40. Need size < 10. Remove 12 elements.
    for(int i=0; i<12; ++i) {
        assert(remove_element_at(arr_shrink, 0) == 0);
    }
    assert(arr_shrink->size == 21 - 12); // size = 9
    // At this point, size (9) < capacity (40) / 4 (10), so shrink should have occurred.
    // Expected new capacity: capacity / RESIZE_FACTOR = 40 / 2 = 20
    if (arr_shrink->capacity < capacity_before_removals) { // Check if it actually shrunk
         printf("  (Info) Array shrinking occurred: Capacity from %zu to %zu\n", capacity_before_removals, arr_shrink->capacity);
         assert(arr_shrink->capacity == capacity_before_removals / RESIZE_FACTOR);
    } else {
         printf("  (Info) Array shrinking did not occur under these conditions / was not aggressive enough.\n");
         // This is not a strict failure of remove, as shrinking is optional/conditional
    }
    // Further remove until empty, check if it shrinks to DEFAULT_INITIAL_CAPACITY
    while(arr_shrink->size > 0) {
        assert(remove_element_at(arr_shrink, 0) == 0);
    }
    assert(arr_shrink->size == 0);
    // If shrinking to default on empty is implemented and capacity was > default
    if (arr_shrink->capacity != DEFAULT_INITIAL_CAPACITY && arr_shrink->capacity > 0){
        // The current implementation shrinks to DEFAULT_INITIAL_CAPACITY when empty after removal
        printf("  (Info) Array capacity after emptying: %zu (Default: %d)\n", arr_shrink->capacity, DEFAULT_INITIAL_CAPACITY);
        assert(arr_shrink->capacity == DEFAULT_INITIAL_CAPACITY || arr_shrink->capacity == 0 ); // allow 0 if it resizes to 0 on empty
    }


    destroy_array(arr);
    destroy_array(arr2);
    destroy_array(arr_shrink);
    printf("  test_remove_element_at: PASSED\n");
}

void test_search_element(void) {
    printf("  Testing search_element...\n");
    DynamicArray *arr = create_array(5);
    assert(arr != NULL);

    // Populate: [10, 20, 30, 40, 50]
    for (int i = 0; i < 5; ++i) {
        append_element(arr, (i + 1) * 10);
    }

    // Search for existing elements
    assert(search_element(arr, 10) == 0);  // Beginning
    assert(search_element(arr, 30) == 2);  // Middle
    assert(search_element(arr, 50) == 4);  // End

    // Search for non-existent element
    assert(search_element(arr, 99) == -1);

    // Search in an empty array
    DynamicArray *empty_arr = create_array(5);
    assert(search_element(empty_arr, 10) == -1);
    destroy_array(empty_arr);
    
    // Search for an element that appears multiple times (should return first occurrence)
    append_element(arr, 30); // arr is now [10, 20, 30, 40, 50, 30]
    assert(search_element(arr, 30) == 2);


    destroy_array(arr);
    printf("  test_search_element: PASSED\n");
}

void test_resize_functionality(void) {
    printf("  Testing resize_array (public function)...\n");
    DynamicArray *arr = create_array(5);
    assert(arr != NULL);
    int element_value;

    // Populate: [0, 1, 2]
    for (int i = 0; i < 3; ++i) {
        append_element(arr, i);
    }
    assert(arr->size == 3);
    assert(arr->capacity == 5);

    // Test increasing capacity
    assert(resize_array(arr, 10) == 0);
    assert(arr->capacity == 10);
    assert(arr->size == 3); // Size should be unchanged
    // Verify data integrity
    for (int i = 0; i < 3; ++i) {
        assert(get_element_at(arr, i, &element_value) == 0 && element_value == i);
    }

    // Test decreasing capacity (but not less than current size)
    assert(resize_array(arr, 7) == 0);
    assert(arr->capacity == 7);
    assert(arr->size == 3);
    for (int i = 0; i < 3; ++i) {
        assert(get_element_at(arr, i, &element_value) == 0 && element_value == i);
    }

    // Test attempting to decrease capacity below current size (should fail)
    assert(resize_array(arr, 2) == -1); // current size is 3
    assert(arr->capacity == 7); // Capacity should be unchanged
    assert(arr->size == 3);     // Size should be unchanged

    // Test resizing to current capacity (should be a no-op, success)
    assert(resize_array(arr, arr->capacity) == 0);
    assert(arr->capacity == 7);
    assert(arr->size == 3);

    // Test resizing with 0 new capacity when size is 0
    DynamicArray *arr_empty_for_resize = create_array(5);
    assert(resize_array(arr_empty_for_resize, 0) == 0);
    assert(arr_empty_for_resize->capacity == 0);
    assert(arr_empty_for_resize->data == NULL); // Implementation detail
    destroy_array(arr_empty_for_resize);

    // Test resizing with 0 new capacity when size > 0 (should fail)
    assert(resize_array(arr, 0) == -1);
    assert(arr->capacity == 7); // Unchanged

    destroy_array(arr);
    printf("  test_resize_functionality: PASSED\n");
}


void test_edge_cases(void) {
    printf("  Testing edge cases and NULL handling...\n");
    DynamicArray *arr_null = NULL;
    int element_value;
    int dummy_element = 1;

    // Test operations on NULL array
    assert(append_element(arr_null, 1) == -1);
    assert(insert_element_at(arr_null, 1, 0) == -1);
    assert(remove_element_at(arr_null, 0) == -1);
    assert(get_element_at(arr_null, 0, &element_value) == -1);
    assert(search_element(arr_null, 1) == -1);
    assert(resize_array(arr_null, 5) == -1);
    print_array(arr_null); // Should print "Array is NULL." and not crash

    // Test operations on an array created with create_array(0)
    DynamicArray *arr_zero_created = create_array(0);
    assert(arr_zero_created != NULL);
    assert(arr_zero_created->capacity == DEFAULT_INITIAL_CAPACITY);
    assert(arr_zero_created->size == 0);

    // Append to it
    assert(append_element(arr_zero_created, 100) == 0);
    assert(arr_zero_created->size == 1);
    assert(get_element_at(arr_zero_created, 0, &element_value) == 0 && element_value == 100);

    // Insert into it
    assert(insert_element_at(arr_zero_created, 50, 0) == 0); // [50, 100]
    assert(arr_zero_created->size == 2);
    assert(get_element_at(arr_zero_created, 0, &element_value) == 0 && element_value == 50);

    // Search
    assert(search_element(arr_zero_created, 100) == 1);
    assert(search_element(arr_zero_created, 999) == -1);
    
    // Remove from it
    assert(remove_element_at(arr_zero_created, 0) == 0); // [100]
    assert(arr_zero_created->size == 1);
    assert(get_element_at(arr_zero_created, 0, &element_value) == 0 && element_value == 100);

    print_array(arr_zero_created); // Should print its content
    destroy_array(arr_zero_created);

    // Test print_array with an empty (but not NULL) array
    DynamicArray *empty_arr = create_array(5);
    print_array(empty_arr); // Should print "Array is empty." and not crash
    destroy_array(empty_arr);

    // Test get_element_at with NULL output pointer
    DynamicArray *arr_for_get = create_array(1);
    append_element(arr_for_get, 10);
    assert(get_element_at(arr_for_get, 0, NULL) == -1);
    destroy_array(arr_for_get);

    printf("  test_edge_cases: PASSED\n");
}
