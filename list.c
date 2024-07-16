#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct nodeStruct* List_createNode(const char *item){

    struct nodeStruct* newNode = malloc(sizeof(struct nodeStruct));
    if (newNode == NULL) {
        return NULL;
    }

    newNode->next = NULL;

    if (item){
        strncpy(newNode->item, item, MAX_ITEM_SIZE);
        newNode->item[MAX_ITEM_SIZE - 1] = '\0';
        newNode->item_size = strlen(newNode->item);
    }
    else{
        newNode->item[0] = '\0';
        newNode->item_size = 0;
    }

    return newNode;
}


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