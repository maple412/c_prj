#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h> // For size_t, malloc, free

// Node structure for the singly linked list
typedef struct Node {
    int data;
    struct Node *next;
} Node;

// LinkedList structure
typedef struct {
    Node *head;     // Pointer to the first node in the list
    size_t size;    // Number of nodes in the list
} LinkedList;

// Function Prototypes

// Creation and Destruction
LinkedList* create_linked_list(void);
void destroy_linked_list(LinkedList* list);

// Modification
int append_node(LinkedList* list, int data);         // Add to end
int prepend_node(LinkedList* list, int data);        // Add to start
int insert_node_at(LinkedList* list, int data, size_t index);
int remove_node_at(LinkedList* list, size_t index);  // Returns 0 on success, -1 on error

// Access and Utility
Node* get_node_at(LinkedList* list, size_t index); // Returns Node pointer or NULL
int search_node(LinkedList* list, int data);       // Returns index or -1 if not found
void print_linked_list(LinkedList* list);
size_t get_list_size(LinkedList* list); // Utility to get current size

#endif // LINKED_LIST_H
