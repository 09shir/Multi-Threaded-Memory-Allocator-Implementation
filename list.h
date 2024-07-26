#pragma once
#include <stddef.h>

#define MAX_ITEM_SIZE 10
struct nodeStruct {
    char item[MAX_ITEM_SIZE]; // pointer to the string
    int item_size;
    struct nodeStruct *next;
};

struct Block {
    void* size;
    struct Block* next;
};

struct Block* List_createBlock(void *item);
void List_insertBlock(struct Block **headRef, struct Block *node);
struct Block* List_searchBlock(struct Block *headRef, void *ptr);
void List_deleteBlock(struct Block **headRef, struct Block *node);
size_t List_getSize_t(void *size);
int List_getInt(void *size);