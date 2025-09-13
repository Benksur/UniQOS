#include "contacts_bptree.h"

/*
    Load btree file if exists, otherwise create new. Returns BPTree struct.
*/
BPTree bptree_create(const char *tree_filename, const char *data_filename)
{
    BPTree tree;

    tree.data_file = fopen(data_filename, "r+b");
    if (!tree.data_file)
        tree.data_file = fopen(data_filename, "w+b");

    tree.tree_file = fopen(tree_filename, "r+b");
    if (!tree.tree_file)
    {
        tree.tree_file = fopen(tree_filename, "w+b");
        // reserve space for root offset at beginning of file
        uint32_t initial_root_offset = sizeof(uint32_t);
        // store root offset at start of file.
        fwrite(&initial_root_offset, sizeof(uint32_t), 1, tree.tree_file);
        // create initial root node (as a leaf since no parent)
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
        // read root offset from existing file
        fseek(tree.tree_file, 0, SEEK_SET);
        if (fread(&tree.root_offset, sizeof(uint32_t), 1, tree.tree_file) != 1)
        {
            tree.root_offset = sizeof(uint32_t); // fallback
        }
    }

    return tree;
}

/*
    Does what it says on the tin.
*/
void bptree_close(BPTree *tree)
{
    if (tree->data_file)
    {
        fclose(tree->data_file);
        tree->data_file = NULL;
    }
    if (tree->tree_file)
    {
        fclose(tree->tree_file);
        tree->tree_file = NULL;
    }
}

/*
    Write contact to data file, return offset it was written at.
*/
uint32_t contacts_append(FILE *data_file, ContactRecord contact)
{
    // Try to reuse a tombstoned slot
    fseek(data_file, 0, SEEK_SET);
    uint32_t offset = 0;
    uint8_t flag;
    while (fread(&flag, 1, 1, data_file) == 1) {
        if (flag == 0) {
            // Found a tombstoned slot, reuse it
            offset = ftell(data_file) - 1;
            break;
        }
        fseek(data_file, sizeof(ContactRecord), SEEK_CUR);
        offset += 1 + sizeof(ContactRecord);
    }
    if (flag != 0) {
        // No tombstoned slot found, append at end
        fseek(data_file, 0, SEEK_END);
        offset = ftell(data_file);
    }
    contact.offset_id = offset;
    flag = 1;
    fseek(data_file, offset, SEEK_SET);
    fwrite(&flag, 1, 1, data_file);
    fwrite(&contact, sizeof(ContactRecord), 1, data_file);
    fflush(data_file);
    return offset;
}

/*
    Read contact from data file at given offset.
    Returns true if contact is active, false if tombstoned.
*/
bool contacts_read(FILE *data_file, uint32_t offset, ContactRecord *out_contact)
{
    fseek(data_file, offset, SEEK_SET);
    uint8_t flag;
    if (fread(&flag, 1, 1, data_file) != 1) return false;
    if (fread(out_contact, sizeof(ContactRecord), 1, data_file) != 1) return false;
    return flag == 1;
}

void contacts_delete(FILE *data_file, uint32_t offset)
{
    fseek(data_file, offset, SEEK_SET);
    uint8_t flag = 0;
    fwrite(&flag, 1, 1, data_file);
    fflush(data_file);
}

/*
    Traverse B+ tree to find leaf node that should contain the given key.
    Returns offset of that leaf node in the tree file.
*/
uint32_t bptree_find_leaf(BPTree *tree, const char *name)
{
    uint32_t current_offset = tree->root_offset;
    BPTreeNode node;

    // traverse the tree to find the appropriate leaf node
    while (1)
    {
        // jump to location of node
        fseek(tree->tree_file, current_offset, SEEK_SET);
        fread(&node, sizeof(BPTreeNode), 1, tree->tree_file);
        if (node.type == LEAF)
        {
            return current_offset;
        }

        int i = 0;
        // keep comparing search term to keys until greater than or end of keys
        while (i < node.key_count && strncmp(name, node.keys[i], MAX_KEY_LEN) > 0)
        {
            i++;
        }
        // set new offset to child location at index i
        current_offset = node.children[i];
    }
}

/*
    Outputs up to CONTACTS_VISIBLE_COUNT names starting from the given offset.
    Since keys are sorted alphabetically, these will also be returned in order.
*/
uint32_t bptree_load_page(BPTree *tree, uint32_t offset, ContactsView *state)
{
    fseek(tree->tree_file, offset, SEEK_SET);
    BPTreeNode node;
    fread(&node, sizeof(BPTreeNode), 1, tree->tree_file);
    // if not leaf, recurse down leftmost branch
    if (node.type == INTERNAL)
    {
        return bptree_load_page(tree, node.children[0], state);
    }
    // now at leaf, read up to CONTACTS_VISIBLE_COUNT contacts
    int count = 0;
    for (int i = 0; i < node.key_count && count < CONTACTS_VISIBLE_COUNT; i++)
    {
        // writes out names and offset ids to state buffer
        ContactRecord contact;
        contacts_read(tree->data_file, node.children[i], &contact);
        strncpy(state->visible[count], contact.name, MAX_NAME_LEN - 1);
        state->offsets[count] = contact.offset_id;
        count++;
    }
    state->visible_count = count;
    return node.next;
}

/*
    Searches for exact key in B+ tree.
    Returns offset in data file if found, otherwise BPTREE_NOT_FOUND.
    Only first search result offset is returned.
*/
ContactRecord bptree_search(BPTree *tree, uint32_t offset)
{
    ContactRecord contact;
    if (contacts_read(tree->data_file, offset, &contact)) {
        return contact;
    }
    // Return a ContactRecord with name_len 0 to indicate not found or tombstoned
    ContactRecord not_found = {0};
    return not_found;
}

/*
    Loads a page of prefix search results starting from the given state.
    Updates state to reflect new position in search.
    Returns number of results loaded into out_names.
*/
int bptree_search_prefix_page(BPTree *tree, PrefixSearchState *state, char out_names[][MAX_NAME_LEN], uint32_t out_offsets[])
{
    int found = 0;
    // continue until we run out of leaves or fill the buffer
    while (state->leaf_offset != 0 && found < CONTACTS_VISIBLE_COUNT)
    {
        fseek(tree->tree_file, state->leaf_offset, SEEK_SET);
        BPTreeNode leaf;
        fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
        // scan keys on current lea
        for (; state->key_index < leaf.key_count && found < CONTACTS_VISIBLE_COUNT; state->key_index++)
        {
            // match result
            if (strncmp(leaf.keys[state->key_index], state->prefix, strlen(state->prefix)) == 0)
            {
                ContactRecord contact;
                contacts_read(tree->data_file, leaf.children[state->key_index], &contact);
                strncpy(out_names[found], contact.name, MAX_NAME_LEN - 1);
                out_offsets[found] = contact.offset_id;
                found++;
            }
        }
        // If finished with this leaf, move to next leaf and reset key_index
        if (state->key_index >= leaf.key_count)
        {
            state->leaf_offset = leaf.next;
            state->key_index = 0;
        }
    }
    return found;
}

/* 
    Find parent of a node by scanning tree.
    Returns parent offset, or 0 if not found (or if child is root). 
*/
static uint32_t bptree_find_parent_recursive(BPTree *tree, uint32_t current_offset, uint32_t child_offset)
{
    if (current_offset == 0 || current_offset == child_offset) return 0;

    fseek(tree->tree_file, current_offset, SEEK_SET);
    BPTreeNode node;
    if (fread(&node, sizeof(BPTreeNode), 1, tree->tree_file) != 1) return 0;

    if (node.type == LEAF) return 0;

    // Check immediate children pointers
    for (int i = 0; i <= node.key_count; i++) {
        if (node.children[i] == child_offset) {
            return current_offset;
        }
    }

    // Recurse into children
    for (int i = 0; i <= node.key_count; i++) {
        uint32_t res = bptree_find_parent_recursive(tree, node.children[i], child_offset);
        if (res != 0) return res;
    }

    return 0;
}

/* Public wrapper: returns parent offset or 0 if none/failure */
static uint32_t bptree_find_parent(BPTree *tree, uint32_t child_offset)
{
    if (tree->root_offset == child_offset) return 0;
    return bptree_find_parent_recursive(tree, tree->root_offset, child_offset);
}


/*
    Adds a new contact to the data file, handle tree balancing.
*/
bool bptree_insert(BPTree *tree, ContactRecord contact)
{
    char key[MAX_KEY_LEN] = {0};
    strncpy(key, contact.name, MAX_KEY_LEN - 1);
    uint32_t data_offset = contacts_append(tree->data_file, contact);
    uint32_t leaf_offset = bptree_find_leaf(tree, key);
    fseek(tree->tree_file, leaf_offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    // leaf has space without need for split and balancing
    if (leaf.key_count < MAX_KEYS)
    {
        int pos = leaf.key_count;
        // find position to insert new key, start at end and shift all lexicographically larger keys right.
        while (pos > 0 && strncmp(key, leaf.keys[pos - 1], MAX_KEY_LEN) < 0)
        {
            strncpy(leaf.keys[pos], leaf.keys[pos - 1], MAX_KEY_LEN);
            leaf.children[pos] = leaf.children[pos - 1];
            pos--;
        }
        // insert new key
        strncpy(leaf.keys[pos], key, MAX_KEY_LEN);
        // we store offset to contact data in children array for leaf nodes
        leaf.children[pos] = data_offset;
        leaf.key_count++;
        // jump back to leaf position
        fseek(tree->tree_file, leaf_offset, SEEK_SET);
        // write updated leaf back to file
        fwrite(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
        fflush(tree->tree_file);
        return true;
    }

    // leaf is full, need to split
    uint32_t new_leaf_offset = bptree_split_leaf(tree, leaf_offset, key, data_offset);
    // get the first key of the new leaf to promote
    fseek(tree->tree_file, new_leaf_offset, SEEK_SET);
    BPTreeNode new_leaf;
    fread(&new_leaf, sizeof(BPTreeNode), 1, tree->tree_file);

    // if this is the root (no parent), create new root
    if (leaf_offset == tree->root_offset)
    {
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
    } else { // has parent, insert promoted key into parent
        uint32_t parent_offset = bptree_find_parent(tree, leaf_offset);
        if (parent_offset == 0) parent_offset = tree->root_offset; // fallback, should not happen
        bptree_insert_internal(tree, parent_offset, new_leaf.keys[0], new_leaf_offset);
    }

    return true;
}

/*
    Helper to update root offset in tree file and in BPTree struct.
    Used when wanting to list contacts in order from start.
*/
uint32_t bptree_get_first_leaf(BPTree *tree)
{
    uint32_t current_offset = tree->root_offset;
    BPTreeNode node;

    while (1)
    {
        fseek(tree->tree_file, current_offset, SEEK_SET);
        fread(&node, sizeof(BPTreeNode), 1, tree->tree_file);

        if (node.type == LEAF)
        {
            return current_offset;
        }

        current_offset = node.children[0];
    }
}

/*
    Does what it says on the tin.
*/
uint32_t bptree_get_next_leaf(BPTree *tree, uint32_t current_leaf_offset)
{
    fseek(tree->tree_file, current_leaf_offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);

    if (leaf.type != LEAF)
    {
        return 0; // Invalid input
    }

    return leaf.next;
}

/*
    Deletes a contact by name from the B+ tree.
    Returns true if deletion was successful, false if not found or error.
    Note: This implementation does not handle merging or redistributing nodes after deletion.
*/
bool bptree_delete(BPTree *tree, ContactRecord contact)
{
    // truncate key for internal comparisons
    char key[MAX_KEY_LEN] = {0};
    strncpy(key, contact.name, MAX_KEY_LEN - 1);
    uint32_t leaf_offset = bptree_find_leaf(tree, key);
    if (leaf_offset == 0) return false;

    fseek(tree->tree_file, leaf_offset, SEEK_SET);
    BPTreeNode leaf;
    if (fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file) != 1) return false;
    // find key in leaf
    int found_index = -1;
    for (int i = 0; i < leaf.key_count; i++) {
        if (strncmp(leaf.keys[i], key, MAX_KEY_LEN) == 0 && leaf.children[i] == contact.offset_id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) { return false; }

    // save the old first key so we can update parent separator if needed
    char old_first_key[MAX_KEY_LEN] = {0};
    if (leaf.key_count > 0) {
        strncpy(old_first_key, leaf.keys[0], MAX_KEY_LEN);
    }

    // remove key and corresponding child pointer by shifting left
    for (int i = found_index; i < leaf.key_count - 1; i++) {
        strncpy(leaf.keys[i], leaf.keys[i + 1], MAX_KEY_LEN);
        leaf.children[i] = leaf.children[i + 1];
    }
    // clear last slot
    memset(leaf.keys[leaf.key_count - 1], 0, MAX_KEY_LEN);
    leaf.children[leaf.key_count - 1] = 0;
    leaf.key_count--;

    // write back updated leaf
    fseek(tree->tree_file, leaf_offset, SEEK_SET);
    fwrite(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    fflush(tree->tree_file);

    contacts_delete(tree->data_file, contact.offset_id);

    // if we removed the first key of the leaf, update parent's separator if present
    if (found_index == 0) {
        // if leaf still has keys, new first key will be leaf.keys[0]; otherwise nothing to update.
        if (leaf.key_count > 0) {
            uint32_t parent_offset = bptree_find_parent(tree, leaf_offset);
            if (parent_offset != 0) {
                fseek(tree->tree_file, parent_offset, SEEK_SET);
                BPTreeNode parent;
                if (fread(&parent, sizeof(BPTreeNode), 1, tree->tree_file) == 1) {
                    // find which key index corresponds to this child pointer and update it.
                    for (int i = 0; i < parent.key_count; i++) {
                        if (parent.children[i] == leaf_offset) {
                            // separator key to the right of children[i] is parent.keys[i]
                            strncpy(parent.keys[i], leaf.keys[0], MAX_KEY_LEN);
                            break;
                        }
                        // also check the last child (children[key_count])
                        if (i == parent.key_count - 1 && parent.children[parent.key_count] == leaf_offset) {
                            // no direct parent.keys entry for final child, typically separator is parent.keys[parent.key_count-1]
                            // adjust that separator to new leaf first key
                            strncpy(parent.keys[parent.key_count - 1], leaf.keys[0], MAX_KEY_LEN);
                            break;
                        }
                    }
                    // write parent back
                    fseek(tree->tree_file, parent_offset, SEEK_SET);
                    fwrite(&parent, sizeof(BPTreeNode), 1, tree->tree_file);
                    fflush(tree->tree_file);
                }
            }
        }
        // if leaf now has zero keys, we do nothing further here.
    }

    return true;
}


/*
    Splits a full leaf node and inserts the new key/data_offset.
    Returns the offset of the newly created leaf node.
*/
uint32_t bptree_split_leaf(BPTree *tree, uint32_t leaf_offset, const char *new_key, uint32_t new_data_offset)
{
    fseek(tree->tree_file, leaf_offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    // create new leaf and have it copy the next pointer of the old leaf.
    BPTreeNode new_leaf = {0};
    new_leaf.type = LEAF;
    new_leaf.key_count = 0;
    new_leaf.next = leaf.next;

    char temp_keys[MAX_KEYS + 1][MAX_KEY_LEN];
    uint32_t temp_children[MAX_KEYS + 1];
    // find insertion position of new key
    int insert_pos = 0;
    while (insert_pos < leaf.key_count &&
           strncmp(new_key, leaf.keys[insert_pos], MAX_KEY_LEN) > 0)
    {
        insert_pos++;
    }
    // copy keys before insertion point
    for (int i = 0; i < insert_pos; i++)
    {
        strncpy(temp_keys[i], leaf.keys[i], MAX_KEY_LEN);
        temp_children[i] = leaf.children[i];
    }
    // insert new key in temp
    strncpy(temp_keys[insert_pos], new_key, MAX_KEY_LEN);
    temp_children[insert_pos] = new_data_offset;
    // copy remaining keys
    for (int i = insert_pos; i < leaf.key_count; i++)
    {
        strncpy(temp_keys[i + 1], leaf.keys[i], MAX_KEY_LEN);
        temp_children[i + 1] = leaf.children[i];
    }
    int total_keys = leaf.key_count + 1;
    int split_point = (total_keys + 1) / 2;

    // update original leaf with first half
    leaf.key_count = split_point;
    for (int i = 0; i < split_point; i++)
    {
        strncpy(leaf.keys[i], temp_keys[i], MAX_KEY_LEN);
        leaf.children[i] = temp_children[i];
    }
    // fill new leaf with second half
    new_leaf.key_count = total_keys - split_point;
    for (int i = 0; i < new_leaf.key_count; i++)
    {
        strncpy(new_leaf.keys[i], temp_keys[split_point + i], MAX_KEY_LEN);
        new_leaf.children[i] = temp_children[split_point + i];
    }
    // write new leaf to end of file
    fseek(tree->tree_file, 0, SEEK_END);
    uint32_t new_leaf_offset = ftell(tree->tree_file);
    fwrite(&new_leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    // update original leaf to point to new leaf
    leaf.next = new_leaf_offset;
    fseek(tree->tree_file, leaf_offset, SEEK_SET);
    fwrite(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    fflush(tree->tree_file);
    return new_leaf_offset;
}

/*
    Inserts a key and child pointer into an internal node, splitting if necessary.
    Returns true on success, false on failure.
*/
bool bptree_insert_internal(BPTree *tree, uint32_t offset, const char *key, uint32_t child_offset)
{
    fseek(tree->tree_file, offset, SEEK_SET);
    BPTreeNode node;
    fread(&node, sizeof(BPTreeNode), 1, tree->tree_file);

    if (node.key_count >= MAX_KEYS)
    {
        // need to split internal node
        bptree_split_internal(tree, offset, key, child_offset);
        return true;
    }

    // find insertion position
    int pos = node.key_count;
    // shift keys and children to make room
    while (pos > 0 && strncmp(key, node.keys[pos - 1], MAX_KEY_LEN) < 0)
    {
        strncpy(node.keys[pos], node.keys[pos - 1], MAX_KEY_LEN);
        node.children[pos + 1] = node.children[pos];
        pos--;
    }
    // insert new key and child
    strncpy(node.keys[pos], key, MAX_KEY_LEN);
    node.children[pos + 1] = child_offset;
    node.key_count++;
    // write back updated node
    fseek(tree->tree_file, offset, SEEK_SET);
    fwrite(&node, sizeof(BPTreeNode), 1, tree->tree_file);
    fflush(tree->tree_file);

    return true;
}

/*
    Updates the root offset in the BPTree struct and adds it to the start of the tree file.
*/
void bptree_update_root(BPTree *tree, uint32_t new_root_offset)
{
    tree->root_offset = new_root_offset;
    // persist the root offset to disk (beginning of file)
    fseek(tree->tree_file, 0, SEEK_SET);
    fwrite(&new_root_offset, sizeof(uint32_t), 1, tree->tree_file);
    fflush(tree->tree_file);
}

/*
    Splits a full internal node and inserts the new key/child_offset.
    Returns the offset of the newly created internal node.
    If the split node was the root, creates a new root as well.
*/
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
    for (int i = 0; i < node.key_count; i++)
    {
        strncpy(temp_keys[i], node.keys[i], MAX_KEY_LEN);
    }
    for (int i = 0; i <= node.key_count; i++)
    {
        temp_children[i] = node.children[i];
    }

    // Find insertion position and insert
    int insert_pos = node.key_count;
    while (insert_pos > 0 && strncmp(key, temp_keys[insert_pos - 1], MAX_KEY_LEN) < 0)
    {
        insert_pos--;
    }

    // Shift keys and children to make room
    for (int i = node.key_count; i > insert_pos; i--)
    {
        strncpy(temp_keys[i], temp_keys[i - 1], MAX_KEY_LEN);
    }
    for (int i = node.key_count + 1; i > insert_pos + 1; i--)
    {
        temp_children[i] = temp_children[i - 1];
    }

    // Insert new key and child
    strncpy(temp_keys[insert_pos], key, MAX_KEY_LEN);
    temp_children[insert_pos + 1] = child_offset;

    int total_keys = node.key_count + 1;
    int split_point = total_keys / 2;

    // Update original node with first half
    node.key_count = split_point;
    for (int i = 0; i < split_point; i++)
    {
        strncpy(node.keys[i], temp_keys[i], MAX_KEY_LEN);
        node.children[i] = temp_children[i];
    }
    node.children[split_point] = temp_children[split_point];

    // Fill new node with second half (skip the middle key as it goes up)
    new_node.key_count = total_keys - split_point - 1;
    for (int i = 0; i < new_node.key_count; i++)
    {
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
    if (node_offset == tree->root_offset)
    {
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

// ...existing code...

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
    const char *names[] = {
        "Eve Adams",
        "Alice Smith",
        "Charlie Wu",
        "Bob Jones",
        "David Chen",
        "Frank Miller",
        "Grace Lee",
        "Henry Davis"};

    int num_names = sizeof(names) / sizeof(names[0]);
    ContactRecord contacts[num_names];

    for (int i = 0; i < num_names; i++)
    {
        memset(&contacts[i], 0, sizeof(ContactRecord));
        strncpy(contacts[i].name, names[i], MAX_NAME_LEN - 1);
        contacts[i].name_len = strlen(contacts[i].name);
        if (!bptree_insert(&tree, contacts[i]))
        {
            printf("  Failed to insert %s\n", names[i]);
            return 1;
        }
    }

    printf("\nTree structure after insertions:\n");
    bptree_debug_print(&tree, tree.root_offset, 0);

    // Test search functionality
    printf("\nTesting search...\n");
    for (int i = 0; i < 3; i++)
    { // Test first few names
        // Find leaf and index for the contact
        uint32_t leaf_offset = bptree_find_leaf(&tree, contacts[i].name);
        fseek(tree.tree_file, leaf_offset, SEEK_SET);
        BPTreeNode leaf;
        fread(&leaf, sizeof(BPTreeNode), 1, tree.tree_file);
        int found_index = -1;
        for (int j = 0; j < leaf.key_count; j++) {
            if (strncmp(leaf.keys[j], contacts[i].name, MAX_KEY_LEN) == 0) {
                found_index = j;
                break;
            }
        }
        if (found_index != -1) {
            uint32_t offset = leaf.children[found_index];
            ContactRecord found = bptree_search(&tree, offset);
            if (found.name_len > 0) {
                printf("  Found: %s (offset: %u)\n", found.name, offset);
            } else {
                printf("  %s not found\n", contacts[i].name);
                return 1;
            }
        } else {
            printf("  %s not found\n", contacts[i].name);
            return 1;
        }
    }

    // Test search for non-existent contact
    uint32_t leaf_offset = bptree_find_leaf(&tree, "Nonexistent Person");
    fseek(tree.tree_file, leaf_offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree.tree_file);
    int found_index = -1;
    for (int j = 0; j < leaf.key_count; j++) {
        if (strncmp(leaf.keys[j], "Nonexistent Person", MAX_KEY_LEN) == 0) {
            found_index = j;
            break;
        }
    }
    uint32_t offset = (found_index != -1) ? leaf.children[found_index] : BPTREE_NOT_FOUND;
    ContactRecord not_found = bptree_search(&tree, offset);
    if (not_found.name_len == 0)
    {
        printf("  Correctly didn't find non-existent contact\n");
    }
    else
    {
        printf("  ERROR: Found non-existent contact\n");
        return 1;
    }

    // Load first page of contacts for UI
    printf("\nLoading first page...\n");
    ContactsView state;
    uint32_t first_leaf = bptree_get_first_leaf(&tree);
    uint32_t next_leaf = bptree_load_page(&tree, first_leaf, &state);
    printf("Loaded %d contacts from first leaf:\n", state.visible_count);
    for (int i = 0; i < state.visible_count; i++)
        printf("  %s\n", state.visible[i]);

    // If there are more leaves, load the next page
    if (next_leaf != 0)
    {
        printf("\nLoading next page (leaf offset: %u)...\n", next_leaf);
        ContactsView state2;
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

    for (int i = 0; i < depth; i++)
        printf("  ");
    printf("Node at %u: type=%s, keys=%d\n", offset,
           node.type == LEAF ? "LEAF" : "INTERNAL", node.key_count);

    for (int i = 0; i < node.key_count; i++)
    {
        for (int j = 0; j < depth + 1; j++)
            printf("  ");
        if (node.type == LEAF)
        {
            ContactRecord contact;
            contacts_read(tree->data_file, node.children[i], &contact);
            printf("Key[%d]: '%s' -> '%s' (offset: %u)\n", i, node.keys[i], contact.name, node.children[i]);
        }
        else
        {
            printf("Key[%d]: '%s'\n", i, node.keys[i]);
        }
    }

    if (node.type == INTERNAL)
    {
        for (int i = 0; i <= node.key_count; i++)
        {
            bptree_debug_print(tree, node.children[i], depth + 1);
        }
    }

    if (node.type == LEAF && node.next != 0)
    {
        for (int i = 0; i < depth; i++)
            printf("  ");
        printf("Next leaf: %u\n", node.next);
    }
}
// ...existing code...

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
    ContactsView state;
    uint32_t next_leaf = bptree_load_page(&tree, tree.root_offset, &state);
    for (int i = 0; i < state.visible_count; i++)
        printf("%s\n", state.visible[i]);

    fclose(tree.data_file);
    fclose(tree.tree_file);
    return 0;
}



PrefixSearchState state = {0};
strncpy(state.prefix, "Al", MAX_KEY_LEN - 1);
state.leaf_offset = bptree_find_leaf(&tree, state.prefix);
state.key_index = 0;


char results[CONTACTS_VISIBLE_COUNT][MAX_NAME_LEN];
int count;

do {
    count = bptree_search_prefix_page(&tree, &state, results);
    for (int i = 0; i < count; i++) {
        printf("%s\n", results[i]);
    }
} while (count == CONTACTS_VISIBLE_COUNT && state.leaf_offset != 0);

*/