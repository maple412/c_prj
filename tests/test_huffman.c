#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/huffman.h" // Adjust path as needed

// Test function prototypes
void test_calculate_frequencies(void);
void test_build_huffman_tree(void); 
void test_generate_huffman_codes(void); 
void test_huffman_encode_decode_simple(void); 
void test_huffman_encode_decode_edge_cases(void); 
void test_free_resources(void); 

// Main function to run all tests
int main() {
    printf("Running Huffman Coding tests...\n");

    test_calculate_frequencies();
    test_build_huffman_tree();
    test_generate_huffman_codes();
    test_huffman_encode_decode_simple();
    test_huffman_encode_decode_edge_cases();
    test_free_resources();

    printf("Huffman Coding tests completed!\n"); // Removed "(many are placeholders)"
    return 0;
}

// Implementations of test functions

void test_calculate_frequencies(void) {
    printf("  Testing calculate_symbol_frequencies...\n");
    const unsigned char data1[] = "aabbc";
    size_t len1 = strlen((const char*)data1);
    unsigned long long* freqs1 = calculate_symbol_frequencies(data1, len1);
    assert(freqs1 != NULL);
    assert(freqs1['a'] == 2);
    assert(freqs1['b'] == 2);
    assert(freqs1['c'] == 1);
    assert(freqs1['d'] == 0);
    free(freqs1);

    const unsigned char data2[] = "";
    size_t len2 = 0;
    unsigned long long* freqs2 = calculate_symbol_frequencies(data2, len2);
    assert(freqs2 != NULL);
    for (int i = 0; i < MAX_SYMBOLS; ++i) {
        assert(freqs2[i] == 0);
    }
    free(freqs2);
    
    const unsigned char data3[] = "aaaaa";
    size_t len3 = 5;
    unsigned long long* freqs3 = calculate_symbol_frequencies(data3, len3);
    assert(freqs3 != NULL);
    assert(freqs3['a'] == 5);
    assert(freqs3['b'] == 0);
    free(freqs3);

    printf("  test_calculate_frequencies: PASSED\n");
}

void test_build_huffman_tree(void) {
    printf("  Testing build_huffman_tree...\n");
    unsigned long long freqs[MAX_SYMBOLS] = {0};
    freqs['a'] = 1; // lowest freq
    freqs['b'] = 2;
    freqs['c'] = 3; // highest freq
    // Expected structure (one possibility, depends on tie-breaking in PQ):
    //     root(6)
    //    /       \
    //  c(3)   internal(3)
    //         /        \
    //       a(1)        b(2)
    HuffmanNode* root = build_huffman_tree(freqs);
    assert(root != NULL);
    assert(root->frequency == 6); 
    // Simple structural check based on typical Huffman algorithm behavior
    // (lowest frequency items tend to be deeper or combined first)
    if (root->left && root->right) {
        if (root->left->frequency == 3 && root->right->frequency == 3) { // c(3) and internal(3)
             assert(root->left->symbol == 'c'); // 'c' is a leaf
             assert(root->right->left != NULL && root->right->right != NULL); // internal node
             if (root->right->left->frequency == 1 && root->right->right->frequency == 2) {
                 assert(root->right->left->symbol == 'a');
                 assert(root->right->right->symbol == 'b');
             } else if (root->right->left->frequency == 2 && root->right->right->frequency == 1) {
                 assert(root->right->left->symbol == 'b');
                 assert(root->right->right->symbol == 'a');
             } else {
                 assert(0); // Failed structural check
             }
        } else if (root->left->frequency == 3 && root->right->frequency == 3) { // internal(3) and c(3)
             assert(root->right->symbol == 'c');
             assert(root->left->left != NULL && root->left->right != NULL);
             if (root->left->left->frequency == 1 && root->left->right->frequency == 2) {
                 assert(root->left->left->symbol == 'a');
                 assert(root->left->right->symbol == 'b');
             } else if (root->left->left->frequency == 2 && root->left->right->frequency == 1) {
                 assert(root->left->left->symbol == 'b');
                 assert(root->left->right->symbol == 'a');
             } else {
                 assert(0); // Failed structural check
             }
        } else {
            // Could be other valid structures due to tie-breaking, e.g. a(1) and internal(5)
            // This simple check is okay for now; full validation via encode/decode.
        }
    } else {
        assert(0); // Root should have two children here
    }
    free_huffman_tree(root);

    // Test with one symbol
    unsigned long long freqs_single[MAX_SYMBOLS] = {0};
    freqs_single['x'] = 100;
    HuffmanNode* root_single = build_huffman_tree(freqs_single);
    assert(root_single != NULL);
    assert(root_single->frequency == 100);
    assert(root_single->left == NULL && root_single->right == NULL); 
    assert(root_single->symbol == 'x');
    free_huffman_tree(root_single);
    
    // Test with no symbols
    unsigned long long freqs_empty[MAX_SYMBOLS] = {0};
    HuffmanNode* root_empty = build_huffman_tree(freqs_empty);
    assert(root_empty == NULL); 
    free_huffman_tree(root_empty);

    // Test with all symbols having same frequency
    unsigned long long freqs_same[MAX_SYMBOLS] = {0};
    freqs_same['a'] = 1; freqs_same['b'] = 1; freqs_same['c'] = 1; freqs_same['d'] = 1;
    HuffmanNode* root_same = build_huffman_tree(freqs_same);
    assert(root_same != NULL);
    assert(root_same->frequency == 4);
    // Tree structure will be balanced.
    free_huffman_tree(root_same);

    printf("  test_build_huffman_tree: PASSED\n");
}

void test_generate_huffman_codes(void) {
    printf("  Testing generate_huffman_codes...\n");
    char* codes_array[MAX_SYMBOLS] = {NULL};

    // Test 1: Known tree structure
    unsigned long long freqs1[MAX_SYMBOLS] = {0};
    freqs1['a'] = 1; // Expect deeper code, e.g., "00"
    freqs1['b'] = 2; // e.g., "01"
    freqs1['c'] = 3; // Expect shallower code, e.g., "1"
    // Possible tree: root(6) -> internal(3) & c(3) -> internal(3) has a(1) & b(2)
    // Codes could be: c:1, a:00, b:01
    HuffmanNode* root1 = build_huffman_tree(freqs1);
    assert(root1 != NULL);
    generate_huffman_codes(root1, codes_array);

    assert(codes_array['a'] != NULL && strlen(codes_array['a']) > 0);
    assert(codes_array['b'] != NULL && strlen(codes_array['b']) > 0);
    assert(codes_array['c'] != NULL && strlen(codes_array['c']) > 0);
    assert(codes_array['d'] == NULL);
    // printf("Code for a: %s, b: %s, c: %s\n", codes_array['a'], codes_array['b'], codes_array['c']);


    // Prefix property check
    for(int i=0; i<MAX_SYMBOLS; ++i) {
        for(int j=0; j<MAX_SYMBOLS; ++j) {
            if (i == j || codes_array[i] == NULL || codes_array[j] == NULL) continue;
            assert(strncmp(codes_array[i], codes_array[j], strlen(codes_array[i])) != 0 || strlen(codes_array[i]) == strlen(codes_array[j]));
        }
    }
    free_huffman_codes(codes_array, MAX_SYMBOLS); // Reset for next test case
    free_huffman_tree(root1);

    // Test 2: Single-node tree
    unsigned long long freqs_single[MAX_SYMBOLS] = {0};
    freqs_single['x'] = 100;
    HuffmanNode* root_single = build_huffman_tree(freqs_single);
    assert(root_single != NULL);
    char* codes_single[MAX_SYMBOLS] = {NULL};
    generate_huffman_codes(root_single, codes_single);
    assert(codes_single['x'] != NULL && strcmp(codes_single['x'], "0") == 0);
    assert(codes_single['y'] == NULL);
    free_huffman_codes(codes_single, MAX_SYMBOLS);
    free_huffman_tree(root_single);

    // Test 3: Empty tree (NULL root)
    char* codes_empty[MAX_SYMBOLS] = {NULL};
    generate_huffman_codes(NULL, codes_empty);
    for(int i=0; i<MAX_SYMBOLS; ++i) assert(codes_empty[i] == NULL);
    // No need to free codes_empty as nothing was allocated.
    
    printf("  test_generate_huffman_codes: PASSED\n");
}

void test_huffman_encode_decode_simple(void) {
    printf("  Testing Huffman encode/decode simple string...\n");
    const unsigned char data[] = "ABRACADABRA"; // A:5, B:2, R:2, C:1, D:1
    size_t data_len = strlen((const char*)data);

    unsigned long long* freqs = calculate_symbol_frequencies(data, data_len);
    assert(freqs != NULL);
    HuffmanNode* root = build_huffman_tree(freqs);
    assert(root != NULL);

    char* codes_array[MAX_SYMBOLS] = {NULL};
    generate_huffman_codes(root, codes_array);
    // Verify some codes exist
    assert(codes_array['A'] != NULL); assert(codes_array['B'] != NULL); assert(codes_array['R'] != NULL);

    size_t encoded_len_bits = 0;
    unsigned char* encoded_data = huffman_encode(data, data_len, codes_array, &encoded_len_bits);
    assert(encoded_data != NULL);
    assert(encoded_len_bits > 0);
    if (data_len > 0) { // Avoid division by zero or meaningless comparison for empty string
         assert(encoded_len_bits < data_len * 8); // Basic check for compression
    }


    size_t decoded_data_len = 0;
    unsigned char* decoded_data = huffman_decode(encoded_data, encoded_len_bits, root, &decoded_data_len);
    assert(decoded_data != NULL);
    assert(decoded_data_len == data_len);
    assert(memcmp(data, decoded_data, data_len) == 0);

    free(freqs);
    free_huffman_codes(codes_array, MAX_SYMBOLS);
    free_huffman_tree(root);
    free(encoded_data);
    free(decoded_data);
    printf("  test_huffman_encode_decode_simple: PASSED\n");
}

void test_huffman_encode_decode_edge_cases(void) {
    printf("  Testing Huffman encode/decode edge cases...\n");
    
    // Test 1: Empty string
    const unsigned char data_empty[] = "";
    size_t data_empty_len = 0;
    unsigned long long* freqs_empty_str = calculate_symbol_frequencies(data_empty, data_empty_len);
    assert(freqs_empty_str != NULL);
    HuffmanNode* root_empty_str = build_huffman_tree(freqs_empty_str); 
    assert(root_empty_str == NULL); // No symbols, no tree
    char* codes_empty[MAX_SYMBOLS] = {NULL};
    generate_huffman_codes(root_empty_str, codes_empty); // Should handle NULL root gracefully

    size_t encoded_len_bits_empty = 0;
    unsigned char* encoded_empty = huffman_encode(data_empty, data_empty_len, codes_empty, &encoded_len_bits_empty);
    assert(encoded_len_bits_empty == 0);
    assert(encoded_empty == NULL); 

    size_t decoded_len_empty = 0;
    unsigned char* decoded_empty = huffman_decode(encoded_empty, encoded_len_bits_empty, root_empty_str, &decoded_len_empty);
    assert(decoded_len_empty == 0);
    assert(decoded_empty == NULL || (decoded_empty != NULL && decoded_empty[0] == '\0')); // Allow NULL or empty string ""

    if(encoded_empty) free(encoded_empty);
    if(decoded_empty) free(decoded_empty);
    free(freqs_empty_str);
    free_huffman_codes(codes_empty, MAX_SYMBOLS);

    // Test 2: Single unique character string (e.g., "aaaaa")
    const unsigned char data_single_char[] = "aaaaa";
    size_t data_single_len = strlen((const char*)data_single_char);
    unsigned long long* freqs_single = calculate_symbol_frequencies(data_single_char, data_single_len);
    assert(freqs_single != NULL);
    HuffmanNode* root_single = build_huffman_tree(freqs_single); 
    assert(root_single != NULL && root_single->left == NULL && root_single->right == NULL && root_single->symbol == 'a');
    
    char* codes_single[MAX_SYMBOLS] = {NULL};
    generate_huffman_codes(root_single, codes_single); 
    assert(codes_single['a'] != NULL && strcmp(codes_single['a'], "0") == 0);

    size_t encoded_len_bits_single = 0;
    unsigned char* encoded_single = huffman_encode(data_single_char, data_single_len, codes_single, &encoded_len_bits_single);
    assert(encoded_single != NULL);
    assert(encoded_len_bits_single == data_single_len * strlen(codes_single['a'])); // Should be 5 * 1 = 5 bits

    size_t decoded_len_single = 0;
    unsigned char* decoded_single = huffman_decode(encoded_single, encoded_len_bits_single, root_single, &decoded_len_single);
    assert(decoded_single != NULL);
    assert(decoded_len_single == data_single_len);
    assert(memcmp(data_single_char, decoded_single, data_single_len) == 0);

    free(freqs_single);
    free_huffman_codes(codes_single, MAX_SYMBOLS);
    free_huffman_tree(root_single);
    free(encoded_single);
    free(decoded_single);

    printf("  test_huffman_encode_decode_edge_cases: PASSED\n");
}

void test_free_resources(void) {
    printf("  Testing free_huffman_tree and free_huffman_codes...\n");
    // Test freeing NULL tree/codes
    free_huffman_tree(NULL);
    free_huffman_codes(NULL, MAX_SYMBOLS);

    char* codes_array[MAX_SYMBOLS] = {NULL};
    codes_array['a'] = strdup("0101"); 
    codes_array['b'] = strdup("11");
    free_huffman_codes(codes_array, MAX_SYMBOLS); 
    assert(codes_array['a'] == NULL); // Check if pointers are NULLed after freeing
    assert(codes_array['b'] == NULL);

    // Build a simple tree, then free it.
    unsigned long long freqs[MAX_SYMBOLS] = {0};
    freqs['x'] = 10; freqs['y'] = 20;
    HuffmanNode* root = build_huffman_tree(freqs);
    assert(root != NULL);
    free_huffman_tree(root); // Valgrind would be best to confirm no leaks.

    printf("  test_free_resources: PASSED\n");
}
