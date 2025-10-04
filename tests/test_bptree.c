/**
 * @file test_bptree.c
 * @brief B+ tree data structure test program
 * @ingroup tests
 *
 * Command-line test program for the B+ tree implementation used for contact storage.
 * Supports adding, searching, listing, and deleting contacts with various operations.
 */

#include "kernel/data_structures/contacts_bptree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <command> [args]\n", argv[0]);
        printf("Commands:\n");
        printf("  add <name> <phone> - Add a contact\n");
        printf("  search <name>      - Search for a contact\n");
        printf("  list               - List all contacts\n");
        printf("  prefix <prefix>    - List contacts matching prefix\n");
        printf("  test               - Run comprehensive test\n");
        printf("  delete <name>      - Delete a contact\n");
        printf("  print              - Print B+ tree structure\n");
        return 1;
    }

    if (strcmp(argv[1], "test") == 0)
    {
        return bptree_test();
    }

    BPTree tree = bptree_create("contacts.dat", "contact_data.dat");

    if (strcmp(argv[1], "add") == 0)
    {
        if (argc < 4)
        {
            printf("Usage: %s add <name> <phone>\n", argv[0]);
            bptree_close(&tree);
            return 1;
        }
        ContactRecord contact = {0};
        strncpy(contact.name, argv[2], MAX_NAME_LEN - 1);
        contact.name_len = strlen(contact.name);
        strncpy(contact.phone, argv[3], MAX_PHONE_LEN - 1);
        contact.phone_len = strlen(contact.phone);
        if (bptree_insert(&tree, contact))
        {
            printf("Added contact: %s, Phone: %s\n", contact.name, contact.phone);
        }
        else
        {
            printf("Failed to add contact: %s\n", contact.name);
        }
    }
    else if (strcmp(argv[1], "search") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: %s search <name>\n", argv[0]);
            bptree_close(&tree);
            return 1;
        }
        char key[MAX_KEY_LEN] = {0};
        strncpy(key, argv[2], MAX_KEY_LEN - 1);
        uint32_t leaf_offset = bptree_find_leaf(&tree, key);
        fseek(tree.tree_file, leaf_offset, SEEK_SET);
        BPTreeNode leaf;
        fread(&leaf, sizeof(BPTreeNode), 1, tree.tree_file);
        int found_index = -1;
        for (int j = 0; j < leaf.key_count; j++)
        {
            if (strncmp(leaf.keys[j], key, MAX_KEY_LEN) == 0)
            {
                found_index = j;
                break;
            }
        }
        uint32_t offset = (found_index != -1) ? leaf.children[found_index] : BPTREE_NOT_FOUND;
        ContactRecord found = bptree_search(&tree, offset);
        if (found.name_len > 0)
        {
            printf("Found contact: %s (offset: %u)\n", found.name, offset);
            printf("  Phone: %s\n", found.phone);
        }
        else
        {
            printf("Contact not found: %s\n", argv[2]);
        }
    }
    else if (strcmp(argv[1], "list") == 0)
    {
        printf("All contacts:\n");
        uint32_t leaf_offset = bptree_get_first_leaf(&tree);
        int total_contacts = 0;

        while (leaf_offset != 0)
        {
            ContactsView state;
            uint32_t next_leaf = bptree_load_page(&tree, leaf_offset, &state);

            for (int i = 0; i < state.visible_count; i++)
            {
                ContactRecord contact;
                contact = bptree_search(&tree, state.offsets[i]);
                printf("  Name: %s, Number: %s, Offset: %u\n", contact.name, contact.phone, contact.offset_id);
                total_contacts++;
            }

            leaf_offset = next_leaf;
        }

        printf("Total contacts: %d\n", total_contacts);
    }
    else if (strcmp(argv[1], "prefix") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: %s prefix <prefix>\n", argv[0]);
            bptree_close(&tree);
            return 1;
        }
        printf("Contacts matching prefix '%s':\n", argv[2]);
        PrefixSearchState state = {0};
        strncpy(state.prefix, argv[2], MAX_KEY_LEN - 1);
        state.leaf_offset = bptree_find_leaf(&tree, state.prefix);
        state.key_index = 0;

        char results[CONTACTS_VISIBLE_COUNT][MAX_NAME_LEN];
        uint32_t results_offsets[CONTACTS_VISIBLE_COUNT];
        int total_found = 0;
        int count;
        do
        {
            count = bptree_search_prefix_page(&tree, &state, results, results_offsets);
            for (int i = 0; i < count; i++)
            {
                printf("  %s\n", results[i]);
                total_found++;
            }
        } while (count == CONTACTS_VISIBLE_COUNT && state.leaf_offset != 0);

        printf("Total matching contacts: %d\n", total_found);
    }
    else if (strcmp(argv[1], "delete") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: %s delete <name>\n", argv[0]);
            bptree_close(&tree);
            return 1;
        }
        // Use prefix search to find first matching contact
        PrefixSearchState state = {0};
        strncpy(state.prefix, argv[2], MAX_KEY_LEN - 1);
        state.leaf_offset = bptree_find_leaf(&tree, state.prefix);
        state.key_index = 0;
        char results[CONTACTS_VISIBLE_COUNT][MAX_NAME_LEN];
        uint32_t results_offsets[CONTACTS_VISIBLE_COUNT];
        int count = bptree_search_prefix_page(&tree, &state, results, results_offsets);
        if (count > 0)
        {
            // Get the first matching contact record
            ContactRecord contact = bptree_search(&tree, results_offsets[0]);
            if (contact.name_len > 0)
            {
                if (bptree_delete(&tree, contact))
                {
                    printf("Deleted contact: %s\n", contact.name);
                }
                else
                {
                    printf("Contact not found or failed to delete: %s\n", argv[2]);
                }
            }
            else
            {
                printf("Contact not found or failed to delete: %s\n", argv[2]);
            }
        }
        else
        {
            printf("Contact not found or failed to delete: %s\n", argv[2]);
        }
    }
    else if (strcmp(argv[1], "print") == 0)
    {
        bptree_debug_print(&tree, tree.root_offset, 0);
    }
    else
    {
        printf("Unknown command: %s\n", argv[1]);
    }

    bptree_close(&tree);
    return 0;
}
