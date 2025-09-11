# B+ Tree Implementation for Contacts

This is a (semi)complete implementation of a B+ Tree data structure designed for storing and managing contacts efficiently. The implementation is file-based and persistent, r/w from SD card to the `contacts_tree.dat` and `contacts_data.dat`.

Refer to following links to get an understanding of how the structure works.
https://www.programiz.com/dsa/b-plus-tree
https://www.cs.usfca.edu/~galles/visualization/BPlusTree.html

## Functionality Summary
This B+ tree implementation uses the first 16 letters of each contact name as the key for indexing and searching. Each leaf node stores up to a fixed number of keys, and for each key, the corresponding child entry is an offset pointing to the actual contact data in the data file. Internal nodes organize the tree for efficient search, while leaf nodes are linked for fast in-order traversal. Each leaf node stores keys in lexicographical (alphabetical) order.

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


## How to Compile and Run Test Suite

```bash
gcc -I./include -o test_bptree tests/test_bptree.c kernel/data_structures/contacts_bptree.c
./test_bptree [command] [arg]
```

## Time Complexities
- **Search:** O(log n)
- **Insert:** O(log n)
- **Delete:** O(log n)
- **List/Traverse:** O(n)

## Current Issues
- Deletion does not merge or rebalance nodes
- Duplicate names are allowed
- If there is a duplicate and you attempt to delete one,
  it will only delete the first instance.
- Case-sensitive search and prefix search
- Fixed maximum key and name lengths
- No thread safety
