#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdlib.h> // For size_t, malloc, realloc, free

// Structure for a dynamic array
typedef struct {
    int *data;      // Pointer to the data elements
    size_t size;    // Current number of elements in the array
    size_t capacity; // Current allocated capacity of the array
} DynamicArray;

// Function Prototypes

// Creation and Destruction
DynamicArray* create_array(size_t initial_capacity);
void destroy_array(DynamicArray* arr);

// Modification
int append_element(DynamicArray* arr, int element); // Add to the end
int insert_element_at(DynamicArray* arr, int element, size_t index);
int remove_element_at(DynamicArray* arr, size_t index); // Returns the removed element or error indicator

// Access and Utility
int get_element_at(DynamicArray* arr, size_t index, int* element_value); // Gets element, returns status
int search_element(DynamicArray* arr, int element); // Returns index or -1 if not found
void print_array(DynamicArray* arr);
int resize_array(DynamicArray* arr, size_t new_capacity); // Internal or exposed utility

#endif // ARRAY_H
