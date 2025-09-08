# B+ Tree Implementation for Contacts

This is a complete implementation of a B+ Tree data structure designed for storing and managing contacts efficiently. The implementation is file-based and persistent, making it suitable for embedded systems and applications requiring reliable data storage.

## Features

- **Persistent Storage**: Data is stored in files and survives program restarts
- **Efficient Search**: O(log n) search time complexity
- **Sorted Access**: Contacts are automatically sorted and can be traversed in order
- **Node Splitting**: Automatic splitting when nodes become full
- **Memory Efficient**: Only loads necessary nodes into memory
- **Error Handling**: Proper error checking and recovery

## File Structure

The B+ tree uses two files:
- **Tree File**: Contains the B+ tree structure (internal nodes and leaf nodes)
- **Data File**: Contains the actual contact data (names)

## API Reference

### Core Functions

#### `BPTree bptree_create(const char *tree_filename, const char *data_filename)`
Creates or opens a B+ tree with the specified files.

#### `void bptree_close(BPTree *tree)`
Closes the B+ tree and frees resources.

#### `bool bptree_insert(BPTree *tree, const char *name)`
Inserts a contact into the B+ tree. Returns `true` on success.

#### `uint32_t bptree_search(BPTree *tree, const char *name)`
Searches for a contact. Returns the data offset or `BPTREE_NOT_FOUND`.

#### `uint32_t bptree_load_page(BPTree *tree, uint32_t offset, ContactsState *state)`
Loads contacts from a leaf node for display purposes.

### Utility Functions

#### `uint32_t bptree_get_first_leaf(BPTree *tree)`
Returns the offset of the first (leftmost) leaf node.

#### `uint32_t bptree_get_next_leaf(BPTree *tree, uint32_t current_leaf_offset)`
Returns the offset of the next leaf node in the chain.

#### `void contacts_read(FILE *data_file, uint32_t offset, char *out_name)`
Reads contact data from the data file at the specified offset.

### Debug Functions

#### `void bptree_debug_print(BPTree *tree, uint32_t offset, int depth)`
Prints the tree structure for debugging purposes.

#### `int bptree_test()`
Runs comprehensive tests on the B+ tree implementation.

## Configuration

The following constants can be modified in `contacts_bptree.h`:

- `MAX_KEYS`: Maximum number of keys per node (default: 4)
- `MAX_KEY_LEN`: Maximum length of a key/name (default: 16)
- `MAX_NAME_LEN`: Maximum length of contact names (default: 64)
- `CONTACTS_VISIBLE_COUNT`: Number of contacts to display per page (default: 8)

## Usage Examples

### Basic Usage

```c
#include "kernel/data_structures/contacts_bptree.h"

int main() {
    // Create/open B+ tree
    BPTree tree = bptree_create("tree.dat", "data.dat");
    
    // Insert contacts
    bptree_insert(&tree, "Alice Smith");
    bptree_insert(&tree, "Bob Jones");
    bptree_insert(&tree, "Charlie Brown");
    
    // Search for a contact
    uint32_t offset = bptree_search(&tree, "Bob Jones");
    if (offset != BPTREE_NOT_FOUND) {
        char name[MAX_NAME_LEN];
        contacts_read(tree.data_file, offset, name);
        printf("Found: %s\\n", name);
    }
    
    // List all contacts
    uint32_t leaf_offset = bptree_get_first_leaf(&tree);
    while (leaf_offset != 0) {
        ContactsState state;
        uint32_t next_leaf = bptree_load_page(&tree, leaf_offset, &state);
        
        for (int i = 0; i < state.visible_count; i++) {
            printf("%s\\n", state.visible[i]);
        }
        
        leaf_offset = next_leaf;
    }
    
    // Clean up
    bptree_close(&tree);
    return 0;
}
```

### Command Line Tool

A complete command-line example is provided in `examples/bptree_example.c`:

```bash
# Compile the example
gcc -I./include -o bptree_example examples/bptree_example.c kernel/data_structures/contacts_bptree.c

# Add contacts
./bptree_example add "John Doe"
./bptree_example add "Jane Smith"

# Search for contacts
./bptree_example search "John Doe"

# List all contacts
./bptree_example list

# Run comprehensive tests
./bptree_example test
```

## Implementation Details

### Node Structure

Each node contains:
- Keys array for storing contact names (up to MAX_KEYS)
- Children array for storing pointers (file offsets)
- Node type (INTERNAL or LEAF)
- Key count
- Next pointer (for leaf nodes only)

### Splitting Algorithm

When a node becomes full (reaches MAX_KEYS), it's split into two nodes:
- **Leaf splitting**: Keys are divided roughly in half, with the first key of the new leaf promoted to the parent
- **Internal splitting**: Keys are divided with the middle key promoted to the parent

### File Format

**Tree File:**
- First 4 bytes: Root node offset
- Followed by node data

**Data File:**
- Variable-length records: [length][name_data]

### Performance Characteristics

- **Search**: O(log n) where n is the number of contacts
- **Insert**: O(log n) average case
- **Space**: Efficient storage with node packing
- **Disk I/O**: Minimal due to tree structure

## Testing

Run the comprehensive test suite:

```bash
gcc -I./include -o test_bptree tests/test_bptree.c kernel/data_structures/contacts_bptree.c
./test_bptree
```

The test suite includes:
- Basic insertion and search operations
- Node splitting scenarios
- Tree structure validation
- Edge cases and error conditions

## Limitations and Future Improvements

### Current Limitations
- Deletion is not implemented
- Fixed maximum key length
- Single-threaded access only

### Planned Improvements
- Implement deletion with node merging
- Add support for variable-length keys
- Thread-safe operations
- Bulk loading operations
- Range queries

## Error Handling

The implementation includes comprehensive error handling:
- File I/O errors are handled gracefully
- Invalid inputs return appropriate error codes
- Memory allocation failures are checked
- Corrupted data is detected where possible

## Integration

This B+ tree is designed to integrate with the UniQOS embedded operating system but can be adapted for other environments. The main dependencies are:
- Standard C library (stdio, stdlib, string)
- File system support

For embedded systems without file systems, the implementation can be adapted to use:
- Flash memory directly
- RAM-based storage
- Network storage backends
