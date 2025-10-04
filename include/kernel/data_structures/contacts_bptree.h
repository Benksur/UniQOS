/**
 * @file contacts_bptree.h
 * @brief B+ tree implementation for contact storage
 * @ingroup contacts_bptree
 *
 * Efficient disk-based B+ tree for storing and searching contacts.
 * Provides fast prefix search and sorted iteration through contacts.
 * The tree is persisted to disk files for non-volatile storage.
 */

#ifndef CONTACTS_BPTREE_C
#define CONTACTS_BPTREE_C

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "sms_types.h"

/** @ingroup contacts_bptree
 *  @brief Maximum keys per B+ tree node */
#define MAX_KEYS 4

/** @ingroup contacts_bptree
 *  @brief Maximum key length in characters */
#define MAX_KEY_LEN 16

/** @ingroup contacts_bptree
 *  @brief Maximum contact name length */
#define MAX_NAME_LEN 64

/** @ingroup contacts_bptree
 *  @brief Maximum phone number length */
#define MAX_PHONE_LEN SMS_MAX_PHONE_LENGTH

/** @ingroup contacts_bptree
 *  @brief Number of contacts visible in UI at once */
#define CONTACTS_VISIBLE_COUNT 9

/** @ingroup contacts_bptree
 *  @brief Sentinel value indicating contact not found */
#define BPTREE_NOT_FOUND UINT32_MAX

/**
 * @brief B+ tree node type
 * @ingroup contacts_bptree
 */
typedef enum
{
    INTERNAL, /**< Internal node (contains keys and child pointers) */
    LEAF      /**< Leaf node (contains keys and data pointers) */
} NodeType;

/**
 * @brief B+ tree node structure
 * @ingroup contacts_bptree
 *
 * Represents a single node in the B+ tree. Can be either an internal
 * node or a leaf node depending on the type field.
 */
typedef struct
{
    char keys[MAX_KEYS][MAX_KEY_LEN]; /**< Search keys stored in this node */
    uint32_t children[MAX_KEYS + 1];  /**< Child node offsets (internal) or data offsets (leaf) */
    uint32_t next;                    /**< Offset to next leaf node (leaves only) */
    NodeType type;                    /**< Node type (INTERNAL or LEAF) */
    uint16_t key_count;               /**< Number of keys currently in node */

} BPTreeNode;

/**
 * @brief B+ tree handle structure
 * @ingroup contacts_bptree
 *
 * Main handle for a B+ tree instance. Contains file handles for
 * tree structure and contact data storage.
 */
typedef struct
{
    FILE *tree_file;      /**< File handle for tree structure */
    FILE *data_file;      /**< File handle for contact data */
    uint32_t root_offset; /**< File offset of root node */
} BPTree;

/**
 * @brief Prefix search state structure
 * @ingroup contacts_bptree
 *
 * Maintains state for iterating through contacts matching a prefix.
 * Used by search functions to resume searches across multiple calls.
 */
typedef struct
{
    uint32_t leaf_offset;     /**< Current leaf node offset */
    int key_index;            /**< Current key index within leaf */
    char prefix[MAX_KEY_LEN]; /**< Search prefix string */
} PrefixSearchState;

/**
 * @brief Contacts view structure
 * @ingroup contacts_bptree
 *
 * Holds a window of visible contacts for UI display.
 * Used to show a subset of contacts efficiently.
 */
typedef struct
{
    char visible[CONTACTS_VISIBLE_COUNT][MAX_NAME_LEN]; /**< Visible contact names */
    uint32_t offsets[CONTACTS_VISIBLE_COUNT];           /**< File offsets for each contact */
    int visible_count;                                  /**< Number of contacts in view */
} ContactsView;

/**
 * @brief Contact record structure
 * @ingroup contacts_bptree
 *
 * Complete contact information stored in the B+ tree data file.
 */
typedef struct
{
    uint8_t name_len;          /**< Length of name string */
    char name[MAX_NAME_LEN];   /**< Contact name */
    uint8_t phone_len;         /**< Length of phone number */
    char phone[MAX_PHONE_LEN]; /**< Phone number */
    uint32_t offset_id;        /**< Unique offset identifier */
} ContactRecord;

// Function declarations
BPTree bptree_create(const char *tree_filename, const char *data_filename);
void bptree_close(BPTree *tree);
bool bptree_insert(BPTree *tree, ContactRecord contact);
ContactRecord bptree_search(BPTree *tree, uint32_t offset);
int bptree_search_prefix_page(BPTree *tree, PrefixSearchState *state, char out_names[][MAX_NAME_LEN], uint32_t out_offsets[]);
uint32_t bptree_load_page(BPTree *tree, uint32_t offset, ContactsView *state);

// Contact data functions
uint32_t contacts_append(FILE *data_file, ContactRecord contact);
bool contacts_read(FILE *data_file, uint32_t offset, ContactRecord *out_contact);

// Internal functions
uint32_t bptree_find_leaf(BPTree *tree, const char *name);
bool bptree_insert_internal(BPTree *tree, uint32_t offset, const char *key, uint32_t child_offset);
uint32_t bptree_split_leaf(BPTree *tree, uint32_t leaf_offset, const char *new_key, uint32_t new_data_offset);
uint32_t bptree_split_internal(BPTree *tree, uint32_t node_offset, const char *key, uint32_t child_offset);
void bptree_update_root(BPTree *tree, uint32_t new_root_offset);

// Test function
int bptree_test();

// Debug function
void bptree_debug_print(BPTree *tree, uint32_t offset, int depth);

// Additional utility functions
bool bptree_delete(BPTree *tree, ContactRecord contact);
uint32_t bptree_get_first_leaf(BPTree *tree);
uint32_t bptree_get_next_leaf(BPTree *tree, uint32_t current_leaf_offset);

#endif