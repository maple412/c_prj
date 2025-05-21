#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For strlen, strcpy, strcat, strdup, memset, memcmp

// --- Static Helper Function Prototypes ---

// For Min-Priority Queue (Min-Heap of HuffmanNode*)
typedef struct MinPriorityQueue {
    HuffmanNode** array; // Array of HuffmanNode pointers
    int size;
    int capacity;
} MinPriorityQueue;

static MinPriorityQueue* create_min_priority_queue(int capacity);
static void destroy_min_priority_queue(MinPriorityQueue* pq);
static void swap_huffman_nodes_in_pq(HuffmanNode** a, HuffmanNode** b); // Utility for heap operations
static void min_heapify(MinPriorityQueue* pq, int idx);
// static void build_min_heap(MinPriorityQueue* pq); // Not strictly needed if inserting one by one
static HuffmanNode* extract_min_node_from_pq(MinPriorityQueue* pq);
static void insert_node_to_pq(MinPriorityQueue* pq, HuffmanNode* node);
static bool is_pq_size_one(MinPriorityQueue* pq);

// Helper to create an internal Huffman node
static HuffmanNode* create_huffman_internal_node(HuffmanNode* left, HuffmanNode* right);
// Helper to create a leaf Huffman node
static HuffmanNode* create_huffman_leaf_node(unsigned char symbol, unsigned long long frequency);


// For generating Huffman codes recursively
static void generate_codes_recursive(HuffmanNode* node, char** codes_array, char* current_code_buffer, int depth);

// For freeing the Huffman tree (recursive helper)
static void free_huffman_tree_recursive(HuffmanNode* node);


// --- Min-Priority Queue Static Helper Implementations ---

static MinPriorityQueue* create_min_priority_queue(int capacity) {
    MinPriorityQueue* pq = (MinPriorityQueue*)malloc(sizeof(MinPriorityQueue));
    if (pq == NULL) {
        perror("Failed to allocate MinPriorityQueue");
        return NULL;
    }
    pq->array = (HuffmanNode**)malloc(sizeof(HuffmanNode*) * capacity);
    if (pq->array == NULL) {
        perror("Failed to allocate array for MinPriorityQueue");
        free(pq);
        return NULL;
    }
    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

static void destroy_min_priority_queue(MinPriorityQueue* pq) {
    if (pq == NULL) {
        return;
    }
    free(pq->array); // Does not free HuffmanNodes themselves
    free(pq);
}

static void swap_huffman_nodes_in_pq(HuffmanNode** a, HuffmanNode** b) {
    HuffmanNode* temp = *a;
    *a = *b;
    *b = temp;
}

static void min_heapify(MinPriorityQueue* pq, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < pq->size && pq->array[left]->frequency < pq->array[smallest]->frequency) {
        smallest = left;
    }
    if (right < pq->size && pq->array[right]->frequency < pq->array[smallest]->frequency) {
        smallest = right;
    }

    if (smallest != idx) {
        swap_huffman_nodes_in_pq(&pq->array[idx], &pq->array[smallest]);
        min_heapify(pq, smallest);
    }
}

static HuffmanNode* extract_min_node_from_pq(MinPriorityQueue* pq) {
    if (pq == NULL || pq->size == 0) {
        return NULL;
    }
    HuffmanNode* min_node = pq->array[0];
    pq->array[0] = pq->array[pq->size - 1];
    pq->size--;
    if (pq->size > 0) { // Only heapify if there are elements left
        min_heapify(pq, 0);
    }
    return min_node;
}

static void insert_node_to_pq(MinPriorityQueue* pq, HuffmanNode* h_node) {
    if (pq == NULL || h_node == NULL) {
        return;
    }
    if (pq->size == pq->capacity) {
        // For simplicity, fixed capacity based on MAX_SYMBOLS.
        // Real-world might need dynamic resizing.
        fprintf(stderr, "Priority queue is full. Cannot insert.\n");
        return;
    }
    pq->size++;
    int i = pq->size - 1;
    pq->array[i] = h_node;

    // Fix the min heap property if it is violated
    while (i != 0 && pq->array[(i - 1) / 2]->frequency > pq->array[i]->frequency) {
        swap_huffman_nodes_in_pq(&pq->array[i], &pq->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

static bool is_pq_size_one(MinPriorityQueue* pq) {
    if (pq == NULL) return false;
    return pq->size == 1;
}

// --- Huffman Node Creation Helpers ---
static HuffmanNode* create_huffman_leaf_node(unsigned char symbol, unsigned long long frequency) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (node == NULL) {
        perror("Failed to allocate HuffmanNode (leaf)");
        return NULL;
    }
    node->symbol = symbol;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static HuffmanNode* create_huffman_internal_node(HuffmanNode* left, HuffmanNode* right) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (node == NULL) {
        perror("Failed to allocate HuffmanNode (internal)");
        return NULL;
    }
    node->symbol = '$'; // Placeholder for internal nodes
    node->frequency = left->frequency + right->frequency;
    node->left = left;
    node->right = right;
    return node;
}


// --- Public Function Implementations ---

unsigned long long* calculate_symbol_frequencies(const unsigned char* data, size_t data_len) {
    unsigned long long* freqs = (unsigned long long*)calloc(MAX_SYMBOLS, sizeof(unsigned long long));
    if (freqs == NULL) {
        perror("Failed to allocate frequency array");
        return NULL;
    }
    for (size_t i = 0; i < data_len; ++i) {
        freqs[data[i]]++;
    }
    return freqs;
}

HuffmanNode* build_huffman_tree(unsigned long long* frequencies) {
    if (frequencies == NULL) return NULL;

    MinPriorityQueue* pq = create_min_priority_queue(MAX_SYMBOLS);
    if (pq == NULL) return NULL;

    int symbol_count = 0;
    for (int i = 0; i < MAX_SYMBOLS; ++i) {
        if (frequencies[i] > 0) {
            HuffmanNode* leaf_node = create_huffman_leaf_node((unsigned char)i, frequencies[i]);
            if (leaf_node == NULL) {
                destroy_min_priority_queue(pq); // Clean up partially built PQ
                return NULL; // Propagate error
            }
            insert_node_to_pq(pq, leaf_node);
            symbol_count++;
        }
    }

    if (symbol_count == 0) { // No symbols with frequency > 0
        destroy_min_priority_queue(pq);
        return NULL;
    }
    if (symbol_count == 1) { // Only one unique symbol
        HuffmanNode* single_node = extract_min_node_from_pq(pq);
        // To handle decoding correctly, a single symbol tree might need an internal parent
        // or specific handling in encode/decode. For now, returning the leaf itself.
        // The `generate_huffman_codes` will assign "0" to this.
        destroy_min_priority_queue(pq);
        return single_node;
    }

    while (!is_pq_size_one(pq)) {
        HuffmanNode* left_child = extract_min_node_from_pq(pq);
        HuffmanNode* right_child = extract_min_node_from_pq(pq);

        if (left_child == NULL || right_child == NULL) { // Should not happen if logic is correct
            fprintf(stderr, "Error: PQ extraction failed unexpectedly.\n");
            // Free remaining nodes in PQ and partially built tree parts if any
            if(left_child) free_huffman_tree(left_child);
            if(right_child) free_huffman_tree(right_child);
            while(pq->size > 0) free_huffman_tree(extract_min_node_from_pq(pq));
            destroy_min_priority_queue(pq);
            return NULL;
        }

        HuffmanNode* internal_node = create_huffman_internal_node(left_child, right_child);
        if (internal_node == NULL) {
            // Free extracted children and rest of PQ
            free_huffman_tree(left_child);
            free_huffman_tree(right_child);
            while(pq->size > 0) free_huffman_tree(extract_min_node_from_pq(pq));
            destroy_min_priority_queue(pq);
            return NULL;
        }
        insert_node_to_pq(pq, internal_node);
    }

    HuffmanNode* root = extract_min_node_from_pq(pq);
    destroy_min_priority_queue(pq);
    return root;
}

static void generate_codes_recursive(HuffmanNode* node, char** codes_array, char* current_code_buffer, int depth) {
    if (node == NULL) {
        return;
    }

    // If it's a leaf node
    if (node->left == NULL && node->right == NULL) {
        current_code_buffer[depth] = '\0';
        codes_array[node->symbol] = strdup(current_code_buffer);
        if (codes_array[node->symbol] == NULL) {
            perror("Failed to allocate memory for Huffman code string");
            // This is tricky to handle mid-recursion. Could set a global error flag.
        }
        return;
    }

    // Traverse left
    if (node->left != NULL) {
        current_code_buffer[depth] = '0';
        generate_codes_recursive(node->left, codes_array, current_code_buffer, depth + 1);
    }

    // Traverse right
    if (node->right != NULL) {
        current_code_buffer[depth] = '1';
        generate_codes_recursive(node->right, codes_array, current_code_buffer, depth + 1);
    }
}

void generate_huffman_codes(HuffmanNode* root, char** codes_array) {
    if (root == NULL) {
        return;
    }
    // Special case: tree with only one node (single symbol)
    if (root->left == NULL && root->right == NULL) {
        codes_array[root->symbol] = strdup("0"); // Convention: code "0"
        if (codes_array[root->symbol] == NULL) {
            perror("Failed to allocate memory for single-symbol Huffman code");
        }
        return;
    }

    char current_code_buffer[MAX_SYMBOLS]; // Max code length for 256 symbols is 255.
    memset(current_code_buffer, 0, MAX_SYMBOLS);
    generate_codes_recursive(root, codes_array, current_code_buffer, 0);
}


unsigned char* huffman_encode(const unsigned char* data, size_t data_len, char** huffman_codes, size_t* encoded_len_bits) {
    if (data_len == 0 || huffman_codes == NULL) {
        *encoded_len_bits = 0;
        return NULL; // Or calloc(1,1) if an empty allocated buffer is desired
    }

    // Calculate total bits needed
    *encoded_len_bits = 0;
    for (size_t i = 0; i < data_len; ++i) {
        if (huffman_codes[data[i]] == NULL) {
             fprintf(stderr, "Error: No Huffman code found for symbol %c (ASCII %d).\n", data[i], data[i]);
             // This can happen if the symbol was not in the frequency data used to build the tree,
             // or if the tree had only one symbol and this is a different one.
             // For a single symbol tree, only that symbol should be in 'data'.
             return NULL; // Indicate error
        }
        *encoded_len_bits += strlen(huffman_codes[data[i]]);
    }
    
    if (*encoded_len_bits == 0) { // e.g. data "aaaaa", code for 'a' is "0", so total bits is 5.
                                // If data was present but no codes (e.g. single node tree, code "0"),
                                // this path might be taken if strlen gives 0.
                                // Code for single symbol is "0", so strlen is 1. This path is fine.
        return NULL; // No bits to encode, effectively empty or error
    }


    size_t encoded_data_bytes = (*encoded_len_bits + 7) / 8;
    unsigned char* encoded_data = (unsigned char*)calloc(encoded_data_bytes, sizeof(unsigned char));
    if (encoded_data == NULL) {
        perror("Failed to allocate memory for encoded data");
        *encoded_len_bits = 0;
        return NULL;
    }

    unsigned char current_byte_buffer = 0;
    int bit_count_in_buffer = 0;
    size_t current_byte_idx = 0;

    for (size_t i = 0; i < data_len; ++i) {
        char* code = huffman_codes[data[i]];
        if (code == NULL) { /* Should have been caught above, but defensive */ free(encoded_data); *encoded_len_bits = 0; return NULL;}

        for (size_t j = 0; j < strlen(code); ++j) {
            current_byte_buffer <<= 1;
            if (code[j] == '1') {
                current_byte_buffer |= 1;
            }
            bit_count_in_buffer++;

            if (bit_count_in_buffer == 8) {
                encoded_data[current_byte_idx++] = current_byte_buffer;
                current_byte_buffer = 0;
                bit_count_in_buffer = 0;
            }
        }
    }

    // If there are remaining bits in the buffer, pad and add them
    if (bit_count_in_buffer > 0) {
        current_byte_buffer <<= (8 - bit_count_in_buffer); // Pad with 0s to the right
        encoded_data[current_byte_idx++] = current_byte_buffer;
    }
    
    // Ensure current_byte_idx matches calculated encoded_data_bytes, unless *encoded_len_bits was 0.
    // This is more of a sanity check.
    if (*encoded_len_bits > 0 && current_byte_idx != encoded_data_bytes) {
        // This might indicate an issue with padding or total bits calculation.
        // For example if encoded_len_bits is 8, encoded_data_bytes is 1. current_byte_idx should be 1.
        // If encoded_len_bits is 7, encoded_data_bytes is 1. current_byte_idx should be 1.
    }


    return encoded_data;
}

unsigned char* huffman_decode(const unsigned char* encoded_data, size_t encoded_len_bits, HuffmanNode* huffman_tree_root, size_t* decoded_data_len) {
    *decoded_data_len = 0;
    if (huffman_tree_root == NULL) { // Cannot decode without a tree
        if (encoded_len_bits == 0) return NULL; // Empty input, empty output is fine.
        fprintf(stderr, "Error: Huffman tree is NULL, cannot decode.\n");
        return NULL;
    }

    // Handle empty input data
    if (encoded_len_bits == 0) {
        // If the tree is just a single leaf (meaning original data was all one char, or empty)
        // And encoded_len_bits is 0, this means original data was empty.
        if (huffman_tree_root->left == NULL && huffman_tree_root->right == NULL) {
             return calloc(1,1); // Return an empty, null-terminated string for 0-length decoded data.
        }
        // If tree is more complex but encoded_len_bits is 0, still means empty original data.
        return calloc(1,1);
    }
    
    // Special case: Tree with only one node (single symbol in original data)
    if (huffman_tree_root->left == NULL && huffman_tree_root->right == NULL) {
        // The code for this single symbol is "0". Each bit in encoded_data should be '0'.
        // The length of decoded data is the number of times this "0" code appears.
        // For a single symbol, its code is '0', so each '0' bit is one symbol.
        // The encoded_len_bits represents how many of these '0's were transmitted.
        *decoded_data_len = encoded_len_bits; // Each bit is a symbol.
        unsigned char* decoded_data_single = (unsigned char*)malloc(*decoded_data_len + 1); // +1 for null terminator
        if (decoded_data_single == NULL) {
            perror("Failed to allocate memory for single-symbol decoded data");
            return NULL;
        }
        for (size_t i = 0; i < *decoded_data_len; ++i) {
            // Check if the bit is actually '0' as expected.
            // (encoded_data[i/8] >> (7-(i%8))) & 1 ; // this gets the bit. if it's 1, it's an error.
            // For simplicity, we assume the encoder correctly encoded '0's.
            decoded_data_single[i] = huffman_tree_root->symbol;
        }
        decoded_data_single[*decoded_data_len] = '\0';
        return decoded_data_single;
    }


    // General case
    // Estimate initial capacity. A common compression ratio is ~2x for text.
    // Max possible decoded length if all codes were 1 bit: encoded_len_bits.
    size_t decoded_capacity = (encoded_len_bits / 2) + 128; // Initial heuristic
    if (decoded_capacity == 0 && encoded_len_bits > 0) decoded_capacity = encoded_len_bits; // if very short codes
    if (decoded_capacity == 0) decoded_capacity = 1; // Minimal capacity for empty string + null terminator

    unsigned char* decoded_data = (unsigned char*)malloc(decoded_capacity + 1); // +1 for potential null terminator
    if (decoded_data == NULL) {
        perror("Failed to allocate memory for decoded data");
        return NULL;
    }

    HuffmanNode* current_node = huffman_tree_root;
    size_t current_decoded_len = 0;

    for (size_t i = 0; i < encoded_len_bits; ++i) {
        // Get current bit: byte index is i/8, bit index within byte is i%8
        // Bits are typically read from MSB to LSB of each byte.
        unsigned char byte = encoded_data[i / 8];
        int bit = (byte >> (7 - (i % 8))) & 1;

        if (bit == 0) {
            current_node = current_node->left;
        } else {
            current_node = current_node->right;
        }

        if (current_node == NULL) { // Should not happen in a correctly built tree and valid bit stream
             fprintf(stderr, "Error: Invalid path in Huffman tree during decoding.\n");
             free(decoded_data);
             return NULL;
        }

        if (current_node->left == NULL && current_node->right == NULL) { // Leaf node
            if (current_decoded_len >= decoded_capacity) {
                decoded_capacity = (decoded_capacity == 0) ? 128 : decoded_capacity * 2;
                unsigned char* temp = (unsigned char*)realloc(decoded_data, decoded_capacity + 1);
                if (temp == NULL) {
                    perror("Failed to reallocate memory for decoded data");
                    free(decoded_data);
                    return NULL;
                }
                decoded_data = temp;
            }
            decoded_data[current_decoded_len++] = current_node->symbol;
            current_node = huffman_tree_root; // Reset for next symbol
        }
    }
    
    decoded_data[current_decoded_len] = '\0'; // Null-terminate
    *decoded_data_len = current_decoded_len;
    return decoded_data;
}

// --- Utility Function Implementations ---

static void free_huffman_tree_recursive(HuffmanNode* node) {
    if (node == NULL) {
        return;
    }
    free_huffman_tree_recursive(node->left);
    node->left = NULL; // Good practice
    free_huffman_tree_recursive(node->right);
    node->right = NULL; // Good practice
    free(node);
}

void free_huffman_tree(HuffmanNode* root) {
    free_huffman_tree_recursive(root);
}

void free_huffman_codes(char** codes_array, int num_symbols) {
    if (codes_array == NULL) {
        return;
    }
    for (int i = 0; i < num_symbols; ++i) {
        if (codes_array[i] != NULL) {
            free(codes_array[i]);
            codes_array[i] = NULL; // Good practice
        }
    }
    // Note: This function does not free codes_array itself,
    // as it might have been stack-allocated (e.g., char* codes[MAX_SYMBOLS];).
    // If codes_array was dynamically allocated as char**, caller must free it.
}
