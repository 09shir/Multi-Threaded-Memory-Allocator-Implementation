#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADER_SIZE 8

struct Block* List_createBlock(void *item) {

    struct Block* newBlock = malloc(sizeof(struct Block));
    newBlock->size = item;
    newBlock->next = NULL;
    return newBlock;
}

void List_insertBlock(struct Block **headRef, struct Block *node) {
    struct Block *current = *headRef;

    if (current == NULL) {
        *headRef = node;
        node->next = NULL;
        return;
    }
    // node need to be added in the same sequence as allocated block to simplyfy deallocation
    else{
        struct Block **currentPointer = headRef;
        while(current){ // comparing the node->size with current-> size
            if (node->size<current->size){
                *currentPointer = node;
                node->next=current;
                return;
            }
            else {
                // currentPointer = (char*)current+ sizeof(void*);
                currentPointer = (struct Block **)((char*)current + sizeof(void*));
                current = current->next;
            }
        }
        // if unable to find a node->size < current->size in the previous step, that means the node is the last node
        *currentPointer = node;
        node->next = NULL;
        return;
    }
}

struct Block* List_searchBlock(struct Block *headRef, void *ptr) {
    struct Block *current = headRef;

    while (current) {
        // printf("%p\n", current->size);
        if (current->size == ptr){
            // printf("found block with size: %d\n", List_getInt(current->size - HEADER_SIZE));
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void List_deleteBlock(struct Block **headRef, struct Block *node) {
    struct Block *current = *headRef;
    // if deleting head
    if (current == node){
        *headRef = current->next ? current->next : NULL;
        // printf("deleting head\n");
    }
    // other cases
    else{
        while(current->next){
            if (current->next == node){
                // if deleted node has more nodes connected
                if (current->next->next){
                    struct Block *tmp = current->next;
                    current->next = tmp->next;
                    // printf("deleting middle node\n");
                }
                else{
                    current->next = NULL;
                    // printf("deleting tail\n");
                }
                return;
            }
            current = current->next;
        }
    }
}

size_t List_getSize_t(void *size) {
    return *(size_t *)(size);
}

int List_getInt(void *size) {
    return *(int *)(size);
}