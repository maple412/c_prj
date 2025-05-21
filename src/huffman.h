#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // For bool, though not strictly needed in this header yet

#define MAX_SYMBOLS 256 // For 8-bit characters

// Huffman Tree Node Structure
typedef struct HuffmanNode {
    unsigned char symbol;       // Character/symbol (meaningful for leaf nodes)
    unsigned long long frequency; // Frequency of the symbol (or sum for internal nodes)
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

// Function Prototypes

// 1. Calculate character frequencies from input data
// Returns an array of frequencies (size MAX_SYMBOLS). Caller must free this array.
unsigned long long* calculate_symbol_frequencies(const unsigned char* data, size_t data_len);

// 2. Build Huffman Tree from frequencies
// Input: Array of frequencies (size MAX_SYMBOLS).
// Returns the root of the Huffman Tree. Caller must free the tree using free_huffman_tree.
HuffmanNode* build_huffman_tree(unsigned long long* frequencies);

// 3. Generate Huffman codes from the tree
// Input: root of Huffman tree, codes_array (must be allocated by caller, e.g., char*[MAX_SYMBOLS]).
// Each string in codes_array will be allocated by this function and must be freed by caller (or via free_huffman_codes).
void generate_huffman_codes(HuffmanNode* root, char** codes_array);

// 4. Encode data using Huffman codes
// Input: original data, its length, and the generated Huffman codes.
// Output: encoded_data (byte array, dynamically allocated, caller must free).
//         encoded_len_bits (pointer to size_t, will store total number of bits in encoded data).
// Returns NULL on failure.
unsigned char* huffman_encode(const unsigned char* data, size_t data_len, char** huffman_codes, size_t* encoded_len_bits);

// 5. Decode data using Huffman tree
// Input: encoded data (byte array), total number of bits in encoded data, and the Huffman tree root.
// Output: decoded_data (byte array, dynamically allocated, caller must free).
//         decoded_data_len (pointer to size_t, will store length of decoded data).
// Returns NULL on failure.
unsigned char* huffman_decode(const unsigned char* encoded_data, size_t encoded_len_bits, HuffmanNode* huffman_tree_root, size_t* decoded_data_len);

// Utility functions
void free_huffman_tree(HuffmanNode* root);
void free_huffman_codes(char** codes_array, int num_symbols); // num_symbols often MAX_SYMBOLS

#endif // HUFFMAN_H
