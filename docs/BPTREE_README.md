# B+ Tree Implementation for Contacts

This is a complete implementation of a B+ Tree data structure designed for storing and managing contacts efficiently. The implementation is file-based and persistent, making it suitable for embedded systems and applications requiring reliable data storage.

Refer to following links to get an understanding of how the structure works.
https://www.programiz.com/dsa/b-plus-tree
https://www.cs.usfca.edu/~galles/visualization/BPlusTree.html

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


## How to Compile and Run

```bash
gcc -I./include -o test_bptree tests/test_bptree.c kernel/data_structures/contacts_bptree.c
./test_bptree
```

## Time Complexities
- **Search:** O(log n)
- **Insert:** O(log n)
- **Delete:** O(log n)
- **List/Traverse:** O(n)

## Current Issues
- Deletion does not merge or rebalance nodes
- Duplicate names are allowed
- Case-sensitive search and prefix search
- Fixed maximum key and name lengths
- No thread safety
