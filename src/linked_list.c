#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>

// Static helper function to create a new node
static Node* create_new_node(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Failed to allocate memory for new node");
        return NULL;
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

LinkedList* create_linked_list(void) {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    if (list == NULL) {
        perror("Failed to allocate memory for LinkedList struct");
        return NULL;
    }
    list->head = NULL;
    list->size = 0;
    return list;
}

void destroy_linked_list(LinkedList* list) {
    if (list == NULL) {
        return;
    }
    Node* current = list->head;
    Node* next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    free(list);
}

int append_node(LinkedList* list, int data) {
    if (list == NULL) {
        fprintf(stderr, "Error: Cannot append to a NULL list.\n");
        return -1;
    }
    Node* newNode = create_new_node(data);
    if (newNode == NULL) {
        return -1; // Error message handled by create_new_node
    }

    if (list->head == NULL) { // Empty list
        list->head = newNode;
    } else {
        Node* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    list->size++;
    return 0;
}

int prepend_node(LinkedList* list, int data) {
    if (list == NULL) {
        fprintf(stderr, "Error: Cannot prepend to a NULL list.\n");
        return -1;
    }
    Node* newNode = create_new_node(data);
    if (newNode == NULL) {
        return -1;
    }

    newNode->next = list->head;
    list->head = newNode;
    list->size++;
    return 0;
}

int insert_node_at(LinkedList* list, int data, size_t index) {
    if (list == NULL) {
        fprintf(stderr, "Error: Cannot insert into a NULL list.\n");
        return -1;
    }
    if (index > list->size) {
        fprintf(stderr, "Error: Index (%zu) out of bounds for insertion (size: %zu).\n", index, list->size);
        return -1;
    }

    if (index == 0) {
        return prepend_node(list, data);
    }
    // If index == list->size, it's like append, but we handle it by inserting before NULL
    // This logic also covers append-like behavior correctly.

    Node* newNode = create_new_node(data);
    if (newNode == NULL) {
        return -1;
    }

    Node* current = list->head;
    // Traverse to the node *before* the target index
    for (size_t i = 0; i < index - 1; ++i) {
        current = current->next;
    }

    newNode->next = current->next;
    current->next = newNode;
    list->size++;
    return 0;
}

int remove_node_at(LinkedList* list, size_t index) {
    if (list == NULL) {
        fprintf(stderr, "Error: Cannot remove from a NULL list.\n");
        return -1;
    }
    if (list->head == NULL) { // Empty list
        fprintf(stderr, "Error: Cannot remove from an empty list.\n");
        return -1;
    }
    if (index >= list->size) {
        fprintf(stderr, "Error: Index (%zu) out of bounds for removal (size: %zu).\n", index, list->size);
        return -1;
    }

    Node* node_to_remove = NULL;
    if (index == 0) { // Remove head
        node_to_remove = list->head;
        list->head = list->head->next;
    } else {
        Node* current = list->head;
        // Traverse to the node *before* the target node
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        node_to_remove = current->next;
        current->next = node_to_remove->next;
    }

    free(node_to_remove);
    list->size--;
    return 0;
}

Node* get_node_at(LinkedList* list, size_t index) {
    if (list == NULL || index >= list->size) {
        return NULL;
    }
    Node* current = list->head;
    for (size_t i = 0; i < index; ++i) {
        current = current->next;
    }
    return current;
}

int search_node(LinkedList* list, int data) {
    if (list == NULL) {
        return -1;
    }
    Node* current = list->head;
    for (size_t i = 0; i < list->size; ++i) {
        if (current->data == data) {
            return (int)i; // Found at index i
        }
        current = current->next;
    }
    return -1; // Not found
}

void print_linked_list(LinkedList* list) {
    if (list == NULL) {
        printf("List is NULL.\n");
        return;
    }
    if (list->head == NULL) {
        printf("List is empty. (Size: 0)\n");
        return;
    }
    printf("List (Size: %zu): [", list->size);
    Node* current = list->head;
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf("]\n");
}

size_t get_list_size(LinkedList* list) {
    if (list == NULL) {
        return 0; // As per instruction, return 0 for NULL list
    }
    return list->size;
}
