#ifndef CONTACTS_BPTREE_C
#define CONTACTS_BPTREE_C

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_KEYS 4
#define MAX_KEY_LEN 16
#define MAX_NAME_LEN 64
#define CONTACTS_VISIBLE_COUNT 8

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
    char visible[CONTACTS_VISIBLE_COUNT][MAX_NAME_LEN];
    int visible_count;
} ContactsState;



#endif