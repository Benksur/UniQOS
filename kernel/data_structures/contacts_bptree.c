#include "kernel/data_structures/contacts_bptree.h"

BPTree bptree_create(const char *tree_filename, const char *data_filename)
{
    BPTree tree;
    
    tree.data_file = fopen(data_filename, "r+b");
    if (!tree.data_file)
        tree.data_file = fopen(data_filename, "w+b");

    tree.tree_file = fopen(tree_filename, "r+b");
    if (!tree.tree_file)
    {
        // Creating new tree file
        tree.tree_file = fopen(tree_filename, "w+b");

        // Reserve space for root offset at beginning of file
        uint32_t initial_root_offset = sizeof(uint32_t);
        fwrite(&initial_root_offset, sizeof(uint32_t), 1, tree.tree_file);
        
        // Create initial root node (which is a leaf)
        BPTreeNode root = {0};
        root.type = LEAF;
        root.key_count = 0;
        root.next = 0;
        fwrite(&root, sizeof(BPTreeNode), 1, tree.tree_file);
        fflush(tree.tree_file);
        
        tree.root_offset = initial_root_offset;
    }
    else
    {
        // Read root offset from existing file
        fseek(tree.tree_file, 0, SEEK_SET);
        if (fread(&tree.root_offset, sizeof(uint32_t), 1, tree.tree_file) != 1) {
            tree.root_offset = sizeof(uint32_t); // Default fallback
        }
    }

    return tree;
}

void bptree_close(BPTree *tree)
{
    if (tree->data_file) {
        fclose(tree->data_file);
        tree->data_file = NULL;
    }
    if (tree->tree_file) {
        fclose(tree->tree_file);
        tree->tree_file = NULL;
    }
}

uint32_t contacts_append(FILE *data_file, const char *name)
{
    fseek(data_file, 0, SEEK_END);
    uint32_t offset = ftell(data_file);
    uint8_t len = (uint8_t)strlen(name);
    if (len > MAX_NAME_LEN - 1)
        len = MAX_NAME_LEN - 1;
    fwrite(&len, 1, 1, data_file);
    fwrite(name, 1, len, data_file);
    fflush(data_file);
    return offset;
}

void contacts_read(FILE *data_file, uint32_t offset, char *out_name)
{
    fseek(data_file, offset, SEEK_SET);
    uint8_t len;
    fread(&len, 1, 1, data_file);
    fread(out_name, 1, len, data_file);
    out_name[len] = '\0';
}

uint32_t bptree_find_leaf(BPTree *tree, const char *name)
{
    uint32_t current_offset = tree->root_offset;
    BPTreeNode node;
    
    while (1) {
        fseek(tree->tree_file, current_offset, SEEK_SET);
        fread(&node, sizeof(BPTreeNode), 1, tree->tree_file);
        
        if (node.type == LEAF) {
            return current_offset;
        }
        
        // Find the correct child in internal node
        int i = 0;
        while (i < node.key_count && strncmp(name, node.keys[i], MAX_KEY_LEN) > 0) {
            i++;
        }
        current_offset = node.children[i];
    }
}

uint32_t bptree_load_page(BPTree *tree, uint32_t offset, ContactsState *state)
{
    fseek(tree->tree_file, offset, SEEK_SET);
    BPTreeNode node;
    fread(&node, sizeof(BPTreeNode), 1, tree->tree_file);
    
    // If this is an internal node, find the leftmost leaf
    if (node.type == INTERNAL) {
        return bptree_load_page(tree, node.children[0], state);
    }
    
    // This is a leaf node
    int count = 0;
    for (int i = 0; i < node.key_count && count < CONTACTS_VISIBLE_COUNT; i++)
    {
        contacts_read(tree->data_file, node.children[i], state->visible[count]);
        count++;
    }
    state->visible_count = count;
    return node.next;
}

uint32_t bptree_search(BPTree *tree, const char *name)
{
    char key[MAX_KEY_LEN] = {0};
    strncpy(key, name, MAX_KEY_LEN - 1);
    
    uint32_t leaf_offset = bptree_find_leaf(tree, key);
    
    fseek(tree->tree_file, leaf_offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    
    for (int i = 0; i < leaf.key_count; i++) {
        if (strncmp(leaf.keys[i], key, MAX_KEY_LEN) == 0) {
            return leaf.children[i];
        }
    }
    return BPTREE_NOT_FOUND;
}

// Deprecated function - kept for compatibility
uint32_t bptree_search_leaf(FILE *tree_file,
                            FILE *data_file,
                            uint32_t offset,
                            const char *name)
{
    (void)data_file; // Suppress unused parameter warning
    
    fseek(tree_file, offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree_file);

    for (int i = 0; i < leaf.key_count; i++)
    {
        if (strncmp(leaf.keys[i], name, MAX_KEY_LEN) == 0)
        {
            return leaf.children[i];
        }
    }
    return BPTREE_NOT_FOUND;
}

bool bptree_insert(BPTree *tree, const char *name)
{
    char key[MAX_KEY_LEN] = {0};
    strncpy(key, name, MAX_KEY_LEN - 1);
    uint32_t data_offset = contacts_append(tree->data_file, name);
    
    uint32_t leaf_offset = bptree_find_leaf(tree, key);
    
    // Try to insert in leaf
    fseek(tree->tree_file, leaf_offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    
    if (leaf.key_count < MAX_KEYS) {
        // Simple case: leaf has space
        int pos = leaf.key_count;
        while (pos > 0 && strncmp(key, leaf.keys[pos - 1], MAX_KEY_LEN) < 0) {
            strncpy(leaf.keys[pos], leaf.keys[pos - 1], MAX_KEY_LEN);
            leaf.children[pos] = leaf.children[pos - 1];
            pos--;
        }
        
        strncpy(leaf.keys[pos], key, MAX_KEY_LEN);
        leaf.children[pos] = data_offset;
        leaf.key_count++;
        
        fseek(tree->tree_file, leaf_offset, SEEK_SET);
        fwrite(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
        fflush(tree->tree_file);
        
        return true;
    }
    
    // Leaf is full, need to split
    uint32_t new_leaf_offset = bptree_split_leaf(tree, leaf_offset, key, data_offset);
    
    // Get the first key of the new leaf to promote
    fseek(tree->tree_file, new_leaf_offset, SEEK_SET);
    BPTreeNode new_leaf;
    fread(&new_leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    
    // If this is the root (no parent), create new root
    if (leaf_offset == tree->root_offset) {
        BPTreeNode new_root = {0};
        new_root.type = INTERNAL;
        new_root.key_count = 1;
        strncpy(new_root.keys[0], new_leaf.keys[0], MAX_KEY_LEN);
        new_root.children[0] = leaf_offset;
        new_root.children[1] = new_leaf_offset;
        
        fseek(tree->tree_file, 0, SEEK_END);
        uint32_t new_root_offset = ftell(tree->tree_file);
        fwrite(&new_root, sizeof(BPTreeNode), 1, tree->tree_file);
        fflush(tree->tree_file);
        
        bptree_update_root(tree, new_root_offset);
    }
    // TODO: Handle case where we need to insert into parent internal node
    
    return true;
}

bool bptree_insert_leaf(BPTree *tree, uint32_t offset, const char *name)
{
    // This function is now deprecated, use bptree_insert instead
    (void)offset; // Suppress unused parameter warning
    return bptree_insert(tree, name);
}

// Test function to demonstrate usage
int bptree_test()
{
    printf("Starting B+ Tree test...\n");
    
    // Clean up any existing test files first
    remove("test_bptree.dat");
    remove("test_contacts.dat");
    
    // Create or load B+ tree and contacts files
    BPTree tree = bptree_create("test_bptree.dat", "test_contacts.dat");

    // Insert contacts in non-alphabetical order to test sorting
    printf("Inserting contacts...\n");
    const char* names[] = {
        "Eve Adams",
        "Alice Smith", 
        "Charlie Wu",
        "Bob Jones",
        "David Chen",
        "Frank Miller",
        "Grace Lee",
        "Henry Davis"
    };
    
    int num_names = sizeof(names) / sizeof(names[0]);
    
    for (int i = 0; i < num_names; i++) {
        printf("  Inserting: %s\n", names[i]);
        if (!bptree_insert(&tree, names[i])) {
            printf("  Failed to insert %s\n", names[i]);
            return 1;
        }
    }
    
    printf("\nTree structure after insertions:\n");
    bptree_debug_print(&tree, tree.root_offset, 0);

    // Test search functionality
    printf("\nTesting search...\n");
    for (int i = 0; i < 3; i++) {  // Test first few names
        uint32_t offset = bptree_search(&tree, names[i]);
        if (offset != BPTREE_NOT_FOUND) {
            char name[MAX_NAME_LEN];
            contacts_read(tree.data_file, offset, name);
            printf("  Found: %s (offset: %u)\n", name, offset);
        } else {
            printf("  %s not found\n", names[i]);
            return 1;
        }
    }
    
    // Test search for non-existent contact
    uint32_t offset = bptree_search(&tree, "Nonexistent Person");
    if (offset == BPTREE_NOT_FOUND) {
        printf("  Correctly didn't find non-existent contact\n");
    } else {
        printf("  ERROR: Found non-existent contact\n");
        return 1;
    }

    // Load first page of contacts for UI
    printf("\nLoading first page...\n");
    ContactsState state;
    uint32_t next_leaf = bptree_load_page(&tree, tree.root_offset, &state);
    printf("Loaded %d contacts from first leaf:\n", state.visible_count);
    for (int i = 0; i < state.visible_count; i++)
        printf("  %s\n", state.visible[i]);
    
    // If there are more leaves, load the next page
    if (next_leaf != 0) {
        printf("\nLoading next page (leaf offset: %u)...\n", next_leaf);
        ContactsState state2;
        bptree_load_page(&tree, next_leaf, &state2);
        printf("Loaded %d contacts from second leaf:\n", state2.visible_count);
        for (int i = 0; i < state2.visible_count; i++)
            printf("  %s\n", state2.visible[i]);
    }

    bptree_close(&tree);
    printf("\nB+ Tree test completed successfully.\n");
    return 0;
}

void bptree_debug_print(BPTree *tree, uint32_t offset, int depth)
{
    fseek(tree->tree_file, offset, SEEK_SET);
    BPTreeNode node;
    fread(&node, sizeof(BPTreeNode), 1, tree->tree_file);
    
    for (int i = 0; i < depth; i++) printf("  ");
    printf("Node at %u: type=%s, keys=%d\n", offset, 
           node.type == LEAF ? "LEAF" : "INTERNAL", node.key_count);
    
    for (int i = 0; i < node.key_count; i++) {
        for (int j = 0; j < depth + 1; j++) printf("  ");
        if (node.type == LEAF) {
            char name[MAX_NAME_LEN];
            contacts_read(tree->data_file, node.children[i], name);
            printf("Key[%d]: '%s' -> '%s' (offset: %u)\n", i, node.keys[i], name, node.children[i]);
        } else {
            printf("Key[%d]: '%s'\n", i, node.keys[i]);
        }
    }
    
    if (node.type == INTERNAL) {
        for (int i = 0; i <= node.key_count; i++) {
            bptree_debug_print(tree, node.children[i], depth + 1);
        }
    }
    
    if (node.type == LEAF && node.next != 0) {
        for (int i = 0; i < depth; i++) printf("  ");
        printf("Next leaf: %u\n", node.next);
    }
}

uint32_t bptree_get_first_leaf(BPTree *tree)
{
    uint32_t current_offset = tree->root_offset;
    BPTreeNode node;
    
    while (1) {
        fseek(tree->tree_file, current_offset, SEEK_SET);
        fread(&node, sizeof(BPTreeNode), 1, tree->tree_file);
        
        if (node.type == LEAF) {
            return current_offset;
        }
        
        // Go to leftmost child
        current_offset = node.children[0];
    }
}

uint32_t bptree_get_next_leaf(BPTree *tree, uint32_t current_leaf_offset)
{
    fseek(tree->tree_file, current_leaf_offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    
    if (leaf.type != LEAF) {
        return 0; // Invalid input
    }
    
    return leaf.next;
}

bool bptree_delete(BPTree *tree, const char *name)
{
    // TODO: Implement deletion
    // For now, just return false to indicate not implemented
    (void)tree;
    (void)name;
    return false;
}

uint32_t bptree_split_leaf(BPTree *tree, uint32_t leaf_offset, const char *new_key, uint32_t new_data_offset)
{
    // Read the full leaf
    fseek(tree->tree_file, leaf_offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    
    // Create new leaf node
    BPTreeNode new_leaf = {0};
    new_leaf.type = LEAF;
    new_leaf.key_count = 0;
    new_leaf.next = leaf.next;
    
    // Create temporary arrays to hold all keys and children (including new one)
    char temp_keys[MAX_KEYS + 1][MAX_KEY_LEN];
    uint32_t temp_children[MAX_KEYS + 1];
    
    // Copy existing keys and find insertion position
    int insert_pos = 0;
    while (insert_pos < leaf.key_count && 
           strncmp(new_key, leaf.keys[insert_pos], MAX_KEY_LEN) > 0) {
        insert_pos++;
    }
    
    // Copy keys before insertion point
    for (int i = 0; i < insert_pos; i++) {
        strncpy(temp_keys[i], leaf.keys[i], MAX_KEY_LEN);
        temp_children[i] = leaf.children[i];
    }
    
    // Insert new key
    strncpy(temp_keys[insert_pos], new_key, MAX_KEY_LEN);
    temp_children[insert_pos] = new_data_offset;
    
    // Copy remaining keys
    for (int i = insert_pos; i < leaf.key_count; i++) {
        strncpy(temp_keys[i + 1], leaf.keys[i], MAX_KEY_LEN);
        temp_children[i + 1] = leaf.children[i];
    }
    
    int total_keys = leaf.key_count + 1;
    int split_point = (total_keys + 1) / 2;  // Split roughly in half
    
    // Update original leaf with first half
    leaf.key_count = split_point;
    for (int i = 0; i < split_point; i++) {
        strncpy(leaf.keys[i], temp_keys[i], MAX_KEY_LEN);
        leaf.children[i] = temp_children[i];
    }
    
    // Fill new leaf with second half
    new_leaf.key_count = total_keys - split_point;
    for (int i = 0; i < new_leaf.key_count; i++) {
        strncpy(new_leaf.keys[i], temp_keys[split_point + i], MAX_KEY_LEN);
        new_leaf.children[i] = temp_children[split_point + i];
    }
    
    // Write new leaf to end of file
    fseek(tree->tree_file, 0, SEEK_END);
    uint32_t new_leaf_offset = ftell(tree->tree_file);
    fwrite(&new_leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    
    // Update original leaf to point to new leaf
    leaf.next = new_leaf_offset;
    fseek(tree->tree_file, leaf_offset, SEEK_SET);
    fwrite(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    
    fflush(tree->tree_file);
    
    return new_leaf_offset;
}

bool bptree_insert_internal(BPTree *tree, uint32_t offset, const char *key, uint32_t child_offset)
{
    fseek(tree->tree_file, offset, SEEK_SET);
    BPTreeNode node;
    fread(&node, sizeof(BPTreeNode), 1, tree->tree_file);
    
    if (node.key_count >= MAX_KEYS) {
        // Need to split internal node
        bptree_split_internal(tree, offset, key, child_offset);
        return true;
    }
    
    // Find insertion position
    int pos = node.key_count;
    while (pos > 0 && strncmp(key, node.keys[pos - 1], MAX_KEY_LEN) < 0) {
        strncpy(node.keys[pos], node.keys[pos - 1], MAX_KEY_LEN);
        node.children[pos + 1] = node.children[pos];
        pos--;
    }
    
    strncpy(node.keys[pos], key, MAX_KEY_LEN);
    node.children[pos + 1] = child_offset;
    node.key_count++;
    
    fseek(tree->tree_file, offset, SEEK_SET);
    fwrite(&node, sizeof(BPTreeNode), 1, tree->tree_file);
    fflush(tree->tree_file);
    
    return true;
}

void bptree_update_root(BPTree *tree, uint32_t new_root_offset)
{
    tree->root_offset = new_root_offset;
    // Persist the root offset to disk
    fseek(tree->tree_file, 0, SEEK_SET);
    fwrite(&new_root_offset, sizeof(uint32_t), 1, tree->tree_file);
    fflush(tree->tree_file);
}

uint32_t bptree_split_internal(BPTree *tree, uint32_t node_offset, const char *key, uint32_t child_offset)
{
    // Read the full internal node
    fseek(tree->tree_file, node_offset, SEEK_SET);
    BPTreeNode node;
    fread(&node, sizeof(BPTreeNode), 1, tree->tree_file);
    
    // Create new internal node
    BPTreeNode new_node = {0};
    new_node.type = INTERNAL;
    new_node.key_count = 0;
    
    // Create temporary arrays to hold all keys and children (including new one)
    char temp_keys[MAX_KEYS + 1][MAX_KEY_LEN];
    uint32_t temp_children[MAX_KEYS + 2];
    
    // Copy existing keys and children
    for (int i = 0; i < node.key_count; i++) {
        strncpy(temp_keys[i], node.keys[i], MAX_KEY_LEN);
    }
    for (int i = 0; i <= node.key_count; i++) {
        temp_children[i] = node.children[i];
    }
    
    // Find insertion position and insert
    int insert_pos = node.key_count;
    while (insert_pos > 0 && strncmp(key, temp_keys[insert_pos - 1], MAX_KEY_LEN) < 0) {
        insert_pos--;
    }
    
    // Shift keys and children to make room
    for (int i = node.key_count; i > insert_pos; i--) {
        strncpy(temp_keys[i], temp_keys[i - 1], MAX_KEY_LEN);
    }
    for (int i = node.key_count + 1; i > insert_pos + 1; i--) {
        temp_children[i] = temp_children[i - 1];
    }
    
    // Insert new key and child
    strncpy(temp_keys[insert_pos], key, MAX_KEY_LEN);
    temp_children[insert_pos + 1] = child_offset;
    
    int total_keys = node.key_count + 1;
    int split_point = total_keys / 2;
    
    // Update original node with first half
    node.key_count = split_point;
    for (int i = 0; i < split_point; i++) {
        strncpy(node.keys[i], temp_keys[i], MAX_KEY_LEN);
        node.children[i] = temp_children[i];
    }
    node.children[split_point] = temp_children[split_point];
    
    // Fill new node with second half (skip the middle key as it goes up)
    new_node.key_count = total_keys - split_point - 1;
    for (int i = 0; i < new_node.key_count; i++) {
        strncpy(new_node.keys[i], temp_keys[split_point + 1 + i], MAX_KEY_LEN);
        new_node.children[i] = temp_children[split_point + 1 + i];
    }
    new_node.children[new_node.key_count] = temp_children[total_keys];
    
    // Write nodes to disk
    fseek(tree->tree_file, node_offset, SEEK_SET);
    fwrite(&node, sizeof(BPTreeNode), 1, tree->tree_file);
    
    fseek(tree->tree_file, 0, SEEK_END);
    uint32_t new_node_offset = ftell(tree->tree_file);
    fwrite(&new_node, sizeof(BPTreeNode), 1, tree->tree_file);
    
    fflush(tree->tree_file);
    
    // The middle key that gets promoted up
    char promoted_key[MAX_KEY_LEN];
    strncpy(promoted_key, temp_keys[split_point], MAX_KEY_LEN);
    
    // Create new root if this was the root
    if (node_offset == tree->root_offset) {
        BPTreeNode new_root = {0};
        new_root.type = INTERNAL;
        new_root.key_count = 1;
        strncpy(new_root.keys[0], promoted_key, MAX_KEY_LEN);
        new_root.children[0] = node_offset;
        new_root.children[1] = new_node_offset;
        
        fseek(tree->tree_file, 0, SEEK_END);
        uint32_t new_root_offset = ftell(tree->tree_file);
        fwrite(&new_root, sizeof(BPTreeNode), 1, tree->tree_file);
        fflush(tree->tree_file);
        
        bptree_update_root(tree, new_root_offset);
    }
    
    return new_node_offset;
}

/*
int main()
{
    // Create or load B+ tree and contacts files
    BPTree tree = bptree_create("bptree.dat", "contacts.dat");

    // Insert contacts
    bptree_insert_leaf(&tree, tree.root_offset, "Alice Smith");
    bptree_insert_leaf(&tree, tree.root_offset, "Bob Jones");
    bptree_insert_leaf(&tree, tree.root_offset, "Charlie Wu");
    bptree_insert_leaf(&tree, tree.root_offset, "Eve Adams");

    // Load first page of contacts for UI
    ContactsState state;
    uint32_t next_leaf = bptree_load_page(&tree, tree.root_offset, &state);
    for (int i = 0; i < state.visible_count; i++)
        printf("%s\n", state.visible[i]);

    fclose(tree.data_file);
    fclose(tree.tree_file);
    return 0;
}


*/