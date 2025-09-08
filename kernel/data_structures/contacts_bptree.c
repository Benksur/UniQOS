#include "contacts_bptree.h"

#include "contacts_bptree.h"

BPTree bptree_create(const char *tree_filename, const char *data_filename)
{
    BPTree tree;
    tree.root_offset = 0;

    tree.data_file = fopen(data_filename, "r+b");
    if (!tree.data_file)
        tree.data_file = fopen(data_filename, "w+b");

    tree.tree_file = fopen(tree_filename, "r+b");
    if (!tree.tree_file)
    {
        tree.tree_file = fopen(tree_filename, "w+b");

        BPTreeNode root = {0};
        root.type = LEAF;
        root.key_count = 0;
        root.next = 0;
        fwrite(&root, sizeof(BPTreeNode), 1, tree.tree_file);
        fflush(tree.tree_file);
    }

    return tree;
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
    flush(data_file);
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

uint32_t bptree_load_page(BPTree *tree, uint32_t offset, ContactsState *state)
{
    fseek(tree->tree_file, offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    int count = 0;
    for (int i = 0; i < leaf.key_count && count < CONTACTS_VISIBLE_COUNT; i++)
    {
        contacts_read(tree->data_file, leaf.children[i], state->visible[count]);
        count++;
    }
    state->visible_count = count;
    return leaf.next;
}

uint32_t bptree_search_leaf(FILE *tree_file,
                            FILE *data_file,
                            uint32_t offset,
                            const char *name)
{
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
    return 0;
}

bool bptree_insert_leaf(BPTree *tree, uint32_t offset, const char *name)
{
    fseek(tree->tree_file, offset, SEEK_SET);
    BPTreeNode leaf;
    fread(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);

    if (leaf.key_count >= MAX_KEYS)
    {
        printf("Leaf full! Need to implement splitting.\n");
        return false;
    }

    char key[MAX_KEY_LEN] = {0};
    strncpy(key, name, MAX_KEY_LEN - 1);

    uint32_t data_offset = contacts_append(tree->data_file, name);

    int pos = leaf.key_count;
    while (pos > 0 && strncmp(key, leaf.keys[pos - 1], MAX_KEY_LEN) < 0)
    {
        strncpy(leaf.keys[pos], leaf.keys[pos - 1], MAX_KEY_LEN);
        leaf.children[pos] = leaf.children[pos - 1];
        pos--;
    }

    strncpy(leaf.keys[pos], key, MAX_KEY_LEN);
    leaf.children[pos] = data_offset;
    leaf.key_count++;

    fseek(tree->tree_file, offset, SEEK_SET);
    fwrite(&leaf, sizeof(BPTreeNode), 1, tree->tree_file);
    fflush(tree->tree_file);

    return true;
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