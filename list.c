#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Block* List_createBlock(void *item) {

    struct Block* newBlock = malloc(sizeof(struct Block));
    newBlock->size = item;
    newBlock->next = NULL;
    return newBlock;
}

// insert in ascending order
void List_insertBlock(struct Block **headRef, struct Block *node) {
    struct Block *current = *headRef;

    if (current == NULL || node->size < current->size) {
        node->next = current;
        *headRef = node;
        return;
    }
    while (current->next->size < node->size && current->next != NULL) {
        current = current->next;
        node->next = current->next;
        current->next = node;
    }
}

struct Block* List_searchBlock(struct Block *headRef, struct Block *node) {
    struct Block *current = headRef;

    while (current) {
        if (current->size == node->size){
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
    }
    // other cases
    else{
        while(current->next){
            if (current->next == node){
                // if deleted node has more nodes connected
                if (current->next->next){
                    struct Block *tmp = current->next;
                    current->next = tmp->next;
                }
                else{
                    current->next = NULL;
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
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node){

    node->next = *headRef;
    *headRef = node;
}


void List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node){

    struct nodeStruct* curr = *headRef;

    // in case head is empty
    if (!curr){
        *headRef = node;
        return;
    }

    while (curr->next){
        curr = curr->next;
    }

    curr->next = node;
}


int List_countNodes (struct nodeStruct *head){

    int count = 0;

    struct nodeStruct *curr = head;
    while (curr){
        count++;
        curr = curr->next;
    }
    return count;
}


struct nodeStruct* List_findNode(struct nodeStruct *head, const char *item){

    struct nodeStruct *ret = head;
    while (ret){
        if (!strcmp(ret->item,item)){
            return ret;
        }
        ret = ret->next;
    }
    return NULL;
}

void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node){

    struct nodeStruct* curr = *headRef;

    // if deleting head
    if (curr == node){
        *headRef = curr->next ? curr->next : NULL;
    }
    // other cases
    else{
        while(curr->next){
            if (curr->next == node){
                // if deleted node has more nodes connected
                if (curr->next->next){
                    struct nodeStruct *tmp = curr->next;
                    curr->next = tmp->next;
                }
                else{
                    curr->next = NULL;
                }
                return;
            }
            curr = curr->next;
        }
    }
}


void List_sort(struct nodeStruct **headRef) {

    // sorting is not needed for linked list length of 0 or 1
    if (!*headRef || !(*headRef)->next) {
        return;  
    }

    struct nodeStruct *sorted = NULL;  // start with an empty sorted list
    struct nodeStruct *current = *headRef;  // current node to insert into sorted list
    struct nodeStruct *next;

    // performs insertion sort
    while (current != NULL) {
        next = current->next;
        struct nodeStruct **finder = &sorted;

        // find the correct position to insert current node
        while (*finder != NULL && strcmp((*finder)->item, current->item) < 0) {
            finder = &(*finder)->next;
        }

        current->next = *finder;
        *finder = current;

        current = next;
    }

    *headRef = sorted;
}