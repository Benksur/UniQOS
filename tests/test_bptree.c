#include "kernel/data_structures/contacts_bptree.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        printf("Commands:\n");
        printf("  add <name>     - Add a contact\n");
        printf("  search <name>  - Search for a contact\n");
        printf("  list           - List all contacts\n");
        printf("  test           - Run comprehensive test\n");
        return 1;
    }

    if (strcmp(argv[1], "test") == 0) {
        return bptree_test();
    }

    BPTree tree = bptree_create("contacts.dat", "contact_data.dat");

    if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Usage: %s add <name>\n", argv[0]);
            bptree_close(&tree);
            return 1;
        }
        
        if (bptree_insert(&tree, argv[2])) {
            printf("Added contact: %s\n", argv[2]);
        } else {
            printf("Failed to add contact: %s\n", argv[2]);
        }
    }
    else if (strcmp(argv[1], "search") == 0) {
        if (argc < 3) {
            printf("Usage: %s search <name>\n", argv[0]);
            bptree_close(&tree);
            return 1;
        }
        
        uint32_t offset = bptree_search(&tree, argv[2]);
        if (offset != BPTREE_NOT_FOUND) {
            char name[MAX_NAME_LEN];
            contacts_read(tree.data_file, offset, name);
            printf("Found contact: %s (offset: %u)\n", name, offset);
        } else {
            printf("Contact not found: %s\n", argv[2]);
        }
    }
    else if (strcmp(argv[1], "list") == 0) {
        printf("All contacts:\n");
        uint32_t leaf_offset = bptree_get_first_leaf(&tree);
        int total_contacts = 0;
        
        while (leaf_offset != 0) {
            ContactsState state;
            uint32_t next_leaf = bptree_load_page(&tree, leaf_offset, &state);
            
            for (int i = 0; i < state.visible_count; i++) {
                printf("  %s\n", state.visible[i]);
                total_contacts++;
            }
            
            leaf_offset = next_leaf;
        }
        
        printf("Total contacts: %d\n", total_contacts);
    }
    else {
        printf("Unknown command: %s\n", argv[1]);
    }

    bptree_close(&tree);
    return 0;
}
