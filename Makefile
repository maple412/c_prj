# Makefile for Data Structures and Algorithms Project

# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
LDFLAGS = # Add libraries here if needed, e.g., -lm for math

# Directories
SRC_DIR = src
TEST_DIR = tests

# Source files (used for dependency tracking and potentially for generic rules if we had object files)
# Not strictly needed for current direct compilation, but good for clarity.
GRAPH_SRC = $(SRC_DIR)/graph.c
GRAPH_TRAVERSAL_SRC = $(SRC_DIR)/graph_traversal.c $(GRAPH_SRC)
SHORTEST_PATH_SRC = $(SRC_DIR)/shortest_path.c $(GRAPH_SRC)

# Test Executables
# Ensure paths are correct for where executables will be created and run from.
TEST_EXECS = \
	$(TEST_DIR)/test_array \
	$(TEST_DIR)/test_linked_list \
	$(TEST_DIR)/test_binary_tree \
	$(TEST_DIR)/test_avl_tree \
	$(TEST_DIR)/test_rbtree \
	$(TEST_DIR)/test_btree \
	$(TEST_DIR)/test_huffman \
	$(TEST_DIR)/test_graph_traversal \
	$(TEST_DIR)/test_shortest_path

# Phony targets (targets that don't represent actual files)
.PHONY: all run_tests clean

# Default target: Build all test executables
all: $(TEST_EXECS)

# Target to run all tests
run_tests: $(TEST_EXECS)
	@echo "--- Running All Tests ---"
	$(foreach test,$(TEST_EXECS), \
		@echo ""; \
		@echo "Running $(test)..."; \
		./$(test); \
	)
	@echo ""
	@echo "--- All Tests Completed ---"

# --- Rules for building individual test executables ---

$(TEST_DIR)/test_array: $(SRC_DIR)/array.c $(TEST_DIR)/test_array.c $(SRC_DIR)/array.h
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/array.c $(TEST_DIR)/test_array.c $(LDFLAGS)

$(TEST_DIR)/test_linked_list: $(SRC_DIR)/linked_list.c $(TEST_DIR)/test_linked_list.c $(SRC_DIR)/linked_list.h
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/linked_list.c $(TEST_DIR)/test_linked_list.c $(LDFLAGS)

$(TEST_DIR)/test_binary_tree: $(SRC_DIR)/binary_tree.c $(TEST_DIR)/test_binary_tree.c $(SRC_DIR)/binary_tree.h
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/binary_tree.c $(TEST_DIR)/test_binary_tree.c $(LDFLAGS)

$(TEST_DIR)/test_avl_tree: $(SRC_DIR)/avl_tree.c $(TEST_DIR)/test_avl_tree.c $(SRC_DIR)/avl_tree.h
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/avl_tree.c $(TEST_DIR)/test_avl_tree.c $(LDFLAGS)

$(TEST_DIR)/test_rbtree: $(SRC_DIR)/rbtree.c $(TEST_DIR)/test_rbtree.c $(SRC_DIR)/rbtree.h
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/rbtree.c $(TEST_DIR)/test_rbtree.c $(LDFLAGS)

$(TEST_DIR)/test_btree: $(SRC_DIR)/btree.c $(TEST_DIR)/test_btree.c $(SRC_DIR)/btree.h
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/btree.c $(TEST_DIR)/test_btree.c $(LDFLAGS)

$(TEST_DIR)/test_huffman: $(SRC_DIR)/huffman.c $(TEST_DIR)/test_huffman.c $(SRC_DIR)/huffman.h
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/huffman.c $(TEST_DIR)/test_huffman.c $(LDFLAGS)

# Graph traversal tests depend on graph.c and graph_traversal.c
$(TEST_DIR)/test_graph_traversal: $(GRAPH_SRC) $(SRC_DIR)/graph_traversal.c $(TEST_DIR)/test_graph_traversal.c $(SRC_DIR)/graph.h $(SRC_DIR)/graph_traversal.h
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $(GRAPH_SRC) $(SRC_DIR)/graph_traversal.c $(TEST_DIR)/test_graph_traversal.c $(LDFLAGS)

# Shortest path tests depend on graph.c and shortest_path.c
$(TEST_DIR)/test_shortest_path: $(GRAPH_SRC) $(SRC_DIR)/shortest_path.c $(TEST_DIR)/test_shortest_path.c $(SRC_DIR)/graph.h $(SRC_DIR)/shortest_path.h
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $(GRAPH_SRC) $(SRC_DIR)/shortest_path.c $(TEST_DIR)/test_shortest_path.c $(LDFLAGS)

# Target to clean up build artifacts
clean:
	@echo "Cleaning up..."
	rm -f $(TEST_EXECS)
	# rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o  # If object files were used
	@echo "Cleanup complete."

# End of Makefile
