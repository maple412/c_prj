#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memmove
#include <limits.h> // For INT_MIN (if needed for error indication, though sticking to 0/-1 for now)

#define DEFAULT_INITIAL_CAPACITY 10
#define RESIZE_FACTOR 2

// Forward declaration for static helper function
static int internal_resize_array(DynamicArray* arr, size_t new_capacity);

DynamicArray* create_array(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = DEFAULT_INITIAL_CAPACITY;
    }

    DynamicArray* arr = (DynamicArray*)malloc(sizeof(DynamicArray));
    if (arr == NULL) {
        perror("Failed to allocate memory for DynamicArray struct");
        return NULL;
    }

    arr->data = (int*)malloc(initial_capacity * sizeof(int));
    if (arr->data == NULL) {
        perror("Failed to allocate memory for array data");
        free(arr); // Clean up allocated struct
        return NULL;
    }

    arr->size = 0;
    arr->capacity = initial_capacity;
    return arr;
}

void destroy_array(DynamicArray* arr) {
    if (arr == NULL) {
        return;
    }
    if (arr->data != NULL) {
        free(arr->data);
        arr->data = NULL; // Good practice to avoid double free
    }
    free(arr);
}

// Made static as it's an internal helper
static int internal_resize_array(DynamicArray* arr, size_t new_capacity) {
    if (arr == NULL) {
        fprintf(stderr, "Error: Cannot resize NULL array.\n");
        return -1;
    }

    // It's generally an error to resize smaller than the current number of elements,
    // unless truncation is explicitly desired and handled.
    // For this implementation, new_capacity must accommodate current elements.
    if (new_capacity < arr->size) {
        fprintf(stderr, "Error: New capacity (%zu) is less than current size (%zu).\n", new_capacity, arr->size);
        return -1;
    }
    
    // If new_capacity is 0 and size is 0, allow shrinking to a minimal capacity or free
    if (new_capacity == 0 && arr->size == 0) {
        free(arr->data);
        arr->data = NULL; // Or allocate a minimal block if preferred
        arr->capacity = 0;
        return 0;
    }
     if (new_capacity == 0 && arr->size > 0) {
        fprintf(stderr, "Error: Cannot resize to 0 capacity when array is not empty.\n");
        return -1;
    }


    int* new_data = (int*)realloc(arr->data, new_capacity * sizeof(int));
    if (new_data == NULL && new_capacity > 0) { // realloc(NULL, size) is like malloc(size)
        perror("Failed to reallocate memory for array data");
        // Original data in arr->data is still valid if realloc fails
        return -1;
    }
    
    arr->data = new_data;
    arr->capacity = new_capacity;
    return 0;
}

// Expose resize_array as per header, but it calls the internal one.
// This allows user-controlled resize if needed, while append/insert use internal logic.
int resize_array(DynamicArray* arr, size_t new_capacity) {
    return internal_resize_array(arr, new_capacity);
}


int append_element(DynamicArray* arr, int element) {
    if (arr == NULL) {
        fprintf(stderr, "Error: Cannot append to NULL array.\n");
        return -1;
    }

    if (arr->size == arr->capacity) {
        size_t new_capacity = (arr->capacity == 0) ? DEFAULT_INITIAL_CAPACITY : arr->capacity * RESIZE_FACTOR;
        if (internal_resize_array(arr, new_capacity) != 0) {
            fprintf(stderr, "Error: Failed to resize array for appending.\n");
            return -1;
        }
    }

    arr->data[arr->size] = element;
    arr->size++;
    return 0;
}

int insert_element_at(DynamicArray* arr, int element, size_t index) {
    if (arr == NULL) {
        fprintf(stderr, "Error: Cannot insert into NULL array.\n");
        return -1;
    }

    // Index can be equal to size (for appending-like behavior)
    if (index > arr->size) {
        fprintf(stderr, "Error: Index (%zu) out of bounds for insertion (size: %zu).\n", index, arr->size);
        return -1;
    }

    if (arr->size == arr->capacity) {
        size_t new_capacity = (arr->capacity == 0) ? DEFAULT_INITIAL_CAPACITY : arr->capacity * RESIZE_FACTOR;
        if (internal_resize_array(arr, new_capacity) != 0) {
            fprintf(stderr, "Error: Failed to resize array for insertion.\n");
            return -1;
        }
    }

    // Shift elements to the right: from index to size-1
    // memmove is safer for overlapping regions
    if (index < arr->size) { // Only shift if not inserting at the very end
        memmove(&arr->data[index + 1], &arr->data[index], (arr->size - index) * sizeof(int));
    }

    arr->data[index] = element;
    arr->size++;
    return 0;
}

int remove_element_at(DynamicArray* arr, size_t index) {
    if (arr == NULL) {
        fprintf(stderr, "Error: Cannot remove from NULL array.\n");
        return -1; // Error
    }

    if (arr->size == 0) {
        fprintf(stderr, "Error: Cannot remove from an empty array.\n");
        return -1; // Error
    }

    if (index >= arr->size) {
        fprintf(stderr, "Error: Index (%zu) out of bounds for removal (size: %zu).\n", index, arr->size);
        return -1; // Error
    }

    // Shift elements to the left: from index+1 to size-1
    // memmove is safer for overlapping regions
    if (index < arr->size - 1) { // Only shift if not removing the last element
         memmove(&arr->data[index], &arr->data[index + 1], (arr->size - index - 1) * sizeof(int));
    }
   
    arr->size--;

    // Optional: Shrink array if size is much smaller than capacity
    // Example: if size is less than capacity / (RESIZE_FACTOR * 2) and capacity is greater than DEFAULT_INITIAL_CAPACITY
    if (arr->capacity > DEFAULT_INITIAL_CAPACITY && arr->size < arr->capacity / (RESIZE_FACTOR * 2)) {
        size_t new_capacity = arr->capacity / RESIZE_FACTOR;
        if (new_capacity < DEFAULT_INITIAL_CAPACITY) {
            new_capacity = DEFAULT_INITIAL_CAPACITY;
        }
        // If array becomes empty, resize to minimal or default.
        // internal_resize_array handles new_capacity < arr->size check, 
        // but after reducing size, this should be fine.
        if (arr->size == 0 && new_capacity > 0) { // if empty, can shrink more
             internal_resize_array(arr, DEFAULT_INITIAL_CAPACITY); 
        } else if (arr->size > 0 && new_capacity >= arr->size) {
             internal_resize_array(arr, new_capacity);
        }
        // Ignore error from shrinking, it's an optimization
    }
    return 0; // Success
}


int get_element_at(DynamicArray* arr, size_t index, int* element_value) {
    if (arr == NULL) {
        fprintf(stderr, "Error: Cannot get element from NULL array.\n");
        return -1;
    }
    if (element_value == NULL) {
        fprintf(stderr, "Error: Output variable element_value is NULL.\n");
        return -1;
    }
    if (index >= arr->size) {
        fprintf(stderr, "Error: Index (%zu) out of bounds for get (size: %zu).\n", index, arr->size);
        return -1;
    }

    *element_value = arr->data[index];
    return 0;
}

int search_element(DynamicArray* arr, int element) {
    if (arr == NULL) {
        fprintf(stderr, "Error: Cannot search in NULL array.\n");
        return -1;
    }
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->data[i] == element) {
            return (int)i; // Found at index i
        }
    }
    return -1; // Not found
}

void print_array(DynamicArray* arr) {
    if (arr == NULL) {
        printf("Array is NULL.\n");
        return;
    }
    if (arr->size == 0) {
        printf("Array is empty. (Capacity: %zu)\n", arr->capacity);
        return;
    }
    printf("Array (Size: %zu, Capacity: %zu): [", arr->size, arr->capacity);
    for (size_t i = 0; i < arr->size; ++i) {
        printf("%d", arr->data[i]);
        if (i < arr->size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}
