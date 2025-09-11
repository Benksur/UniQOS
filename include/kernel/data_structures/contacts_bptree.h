#ifndef CONTACTS_BPTREE_C
#define CONTACTS_BPTREE_C

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_KEYS 4
#define MAX_KEY_LEN 16
#define MAX_NAME_LEN 64
#define MAX_PHONE_LEN 10
#define CONTACTS_VISIBLE_COUNT 8
#define BPTREE_NOT_FOUND UINT32_MAX

typedef enum { INTERNAL, LEAF } NodeType;

typedef struct {
    char keys[MAX_KEYS][MAX_KEY_LEN]; 
    uint32_t children[MAX_KEYS + 1];
    uint32_t next; 
    NodeType type;
    uint16_t key_count;
    
} BPTreeNode;

typedef struct {
    FILE* tree_file;
    FILE* data_file;
    uint32_t root_offset;
} BPTree;

typedef struct {
    uint32_t leaf_offset;
    int key_index;
    char prefix[MAX_KEY_LEN];
} PrefixSearchState;


typedef struct {
    char visible[CONTACTS_VISIBLE_COUNT][MAX_NAME_LEN];
    int visible_count;
} ContactsState;


typedef struct {
    uint8_t name_len;
    char name[MAX_NAME_LEN];
    uint8_t phone_len;
    char phone[MAX_PHONE_LEN];
} ContactRecord;

// Function declarations
BPTree bptree_create(const char *tree_filename, const char *data_filename);
void bptree_close(BPTree *tree);
bool bptree_insert(BPTree *tree, const char *name);
uint32_t bptree_search(BPTree *tree, const char *name);
int bptree_search_prefix_page(BPTree *tree, PrefixSearchState *state, char out_names[][MAX_NAME_LEN]);
uint32_t bptree_load_page(BPTree *tree, uint32_t offset, ContactsState *state);

// Contact data functions
uint32_t contacts_append(FILE *data_file, const char *name);
void contacts_read(FILE *data_file, uint32_t offset, char *out_name);

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
bool bptree_delete(BPTree *tree, const char *name);
uint32_t bptree_get_first_leaf(BPTree *tree);
uint32_t bptree_get_next_leaf(BPTree *tree, uint32_t current_leaf_offset);



#endif