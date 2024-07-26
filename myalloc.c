#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myalloc.h"
#include "list.h"
#include <stdbool.h>
#include <pthread.h>

// each linked list should contain 
// 1. a pointer to the allocated memory block
// 2. a pointer to the next node in the list

#define HEADER_SIZE 8

struct Myalloc {
    enum allocation_algorithm aalgorithm;
    size_t size;
    void* memory; // points to head
    struct Block* freeList; //LL
    struct Block* allocatedList; //LL
    pthread_mutex_t lock; 
};

struct Myalloc myalloc;

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);

    // size should align to the nearest next 64-byte boundary (round up)
    myalloc.size = (_size + 63) & ~63;
    myalloc.memory = malloc(myalloc.size);
    myalloc.aalgorithm = _aalgorithm;

    memset(myalloc.memory, 0, myalloc.size);

    struct Block *block = List_createBlock(myalloc.memory + HEADER_SIZE);
    List_insertBlock(&myalloc.freeList, block);

    // copy size into the header
    size_t header_size = myalloc.size - HEADER_SIZE;
    memcpy(myalloc.memory, &header_size, HEADER_SIZE);
    myalloc.allocatedList = NULL;

    pthread_mutex_init(&myalloc.lock, NULL);
}

void destroy_allocator() {
    pthread_mutex_lock(&myalloc.lock);
    free(myalloc.memory);
    struct Block* freeBlock = myalloc.freeList; 
    struct Block* allocatedBlock = myalloc.allocatedList; 
    while(freeBlock){
        struct Block* tmp = freeBlock;
        freeBlock = freeBlock->next;
        free(tmp);
    }
    myalloc.freeList = NULL;
    while(allocatedBlock){
        struct Block* tmp = allocatedBlock;
        allocatedBlock = allocatedBlock->next;
        free(tmp);
    }
    myalloc.allocatedList = NULL;
    myalloc.memory = NULL;
    // Free all nodes in freeList and allocatedList
    // Assuming List_deleteNode manages freeing nodes
    pthread_mutex_unlock(&myalloc.lock);
    pthread_mutex_destroy(&myalloc.lock);
}

// TODO: Best Fit
void* allocate(int _size) {
    void* ptr = NULL;

    // Allocate memory from myalloc.memory 
    // ptr = address of allocated memory

    pthread_mutex_lock(&myalloc.lock);

    int total_size = _size + HEADER_SIZE;
    struct Block *curr = myalloc.freeList;
    struct Block *tmp = NULL;
    struct Block *best_fit = NULL;

    if (myalloc.aalgorithm == FIRST_FIT) {
        while (curr) {
            if (List_getInt(curr->size - HEADER_SIZE) + HEADER_SIZE >= total_size) {
                best_fit = curr;
                break;
            }
            curr = curr->next;
        }
    }
    else if (myalloc.aalgorithm == BEST_FIT) {
        while (curr) {
            int tmp_size = List_getInt(curr->size - HEADER_SIZE) + HEADER_SIZE;
            if (tmp_size == total_size){
                best_fit = curr;
                break;
            }
            else if (!tmp || (tmp_size >= total_size && tmp_size < List_getInt(tmp->size - HEADER_SIZE) + HEADER_SIZE)) {
                tmp = curr;
            }
            curr = curr->next;
        }
        if (!best_fit){
            best_fit = tmp;
        }
    }

    // return null if best_fit not found (memory full)
    if (!best_fit) {
        pthread_mutex_unlock(&myalloc.lock);
        return NULL;
    }

    // a new header will only be added if (free size (header not included) > requested size + HEADER_SIZE)
    if (List_getInt(best_fit->size - HEADER_SIZE) > _size + HEADER_SIZE) {
        // add new header
        size_t header_size = List_getSize_t(best_fit->size - HEADER_SIZE) - _size - HEADER_SIZE;
        memcpy(best_fit->size + _size, &header_size, HEADER_SIZE);

        // push new free block from below
        struct Block* newBlock = List_createBlock(best_fit->size + _size + HEADER_SIZE);
        List_insertBlock(&myalloc.freeList, newBlock);
    }

    // change old header
    *(size_t *) (best_fit->size - HEADER_SIZE) = _size;

    // remove it from freeList
    List_deleteBlock(&myalloc.freeList, best_fit);
    // add it to allocatedList
    List_insertBlock(&myalloc.allocatedList, best_fit);

    ptr = best_fit->size;
    // check if allocatedList has best_fit

    pthread_mutex_unlock(&myalloc.lock);

    return ptr;
}

void deallocate(void* _ptr) {
    assert(_ptr != NULL);

    pthread_mutex_lock(&myalloc.lock);

    struct Block* block_to_remove = List_searchBlock(myalloc.allocatedList, _ptr);

    List_deleteBlock(&myalloc.allocatedList, block_to_remove);
    List_insertBlock(&myalloc.freeList, block_to_remove);

        struct Block* freeBlock = myalloc.freeList;
        
        //solve framentation that are toot small to be included in the free list
        if (myalloc.allocatedList!=NULL){
            
            struct Block* allocatedBlock = myalloc.allocatedList;
            int indicator = 0;
            //check if there is free space immediately on the right hand side of the free block.
            while (allocatedBlock->next){
                if(allocatedBlock->size>block_to_remove->size){//loop until allocated block is on the right hand side of the free block
                    indicator++;}
                if(indicator == 1){ // indicator == 1 means we found the next allocated space ofter free space. 
                    int* freesize = block_to_remove->size-HEADER_SIZE;
                    *freesize = allocatedBlock->size - block_to_remove->size -HEADER_SIZE;//mm debug
                }
                allocatedBlock = allocatedBlock->next;
                }
            // same as above loop, just one more loop without allocatedBlock = allocatedBlock->next
            if(allocatedBlock->size>block_to_remove->size){//loop until allocated block is on the right hand side of the free block
                indicator++;}
            if(indicator == 1){ 
                // indicator == 1 means we found the next allocated space ofter free space. 
                void* firstblockEnd;
                int size = List_getInt(freeBlock->size - HEADER_SIZE);
                firstblockEnd = size + (char*)freeBlock->size;
                if(List_searchBlock(myalloc.allocatedList,firstblockEnd+HEADER_SIZE)==NULL){
                    int* freesize = block_to_remove->size-HEADER_SIZE;
                    *freesize = allocatedBlock->size - freeBlock->size;
                }

            }
            //if the freeblock is pointing to the very last chunk in the memory, merge with the rest of the memory together.
            if(allocatedBlock->size<block_to_remove->size ){
                int* freesize = block_to_remove->size-HEADER_SIZE;
                *freesize = (myalloc.memory+myalloc.size-block_to_remove->size);

            }
        }
        //if there is no allocated block, then entire thing is free.
        else{
            int* freesize = freeBlock->size-HEADER_SIZE;
            *freesize = (myalloc.memory+myalloc.size-freeBlock->size);
        }

        //merge two consecutive free blocks
        while (freeBlock->next) {
            void* firstblockEnd;
            int size = List_getInt(freeBlock->size - HEADER_SIZE);
            firstblockEnd = size + (char*)freeBlock->size;
            void* secondblockStart =  freeBlock->next->size - HEADER_SIZE;
            int secondblockSize = List_getInt(freeBlock->next->size - HEADER_SIZE) + HEADER_SIZE;
            void* secondblockEnd = secondblockSize + (char*)freeBlock->next->size - HEADER_SIZE;
            if (firstblockEnd == secondblockStart){
                int* sizeptr = freeBlock->size-HEADER_SIZE ;
                size = *sizeptr;
                size = size + List_getInt(secondblockStart) + HEADER_SIZE;
                *sizeptr = size;
                struct Block* deletePointer = freeBlock->next;
                List_deleteBlock(&myalloc.freeList, freeBlock->next);
                free(deletePointer);
            }
            else if (firstblockEnd == secondblockEnd) {
                struct Block* deletePointer = freeBlock->next;
                List_deleteBlock(&myalloc.freeList, freeBlock->next);
                free(deletePointer);
            }
            else freeBlock = freeBlock->next;
        }

        pthread_mutex_unlock(&myalloc.lock);
    
}

int compact_allocation(void** _before, void** _after) {
    pthread_mutex_lock(&myalloc.lock);
    int compacted_size = 0;
    struct Block* allocatedBlock = myalloc.allocatedList;
    int chunksize;
    void* chunkAddress;
    void* chunkNewAddress= myalloc.memory;
    while(allocatedBlock){
        chunkAddress = allocatedBlock->size - HEADER_SIZE;
        chunksize = List_getInt(chunkAddress)+HEADER_SIZE;
        compact_allocation_helper(chunkAddress,chunksize, chunkNewAddress);
        //update before/after
        _before[compacted_size] = chunkAddress;
        _after[compacted_size] = chunkNewAddress;
        compacted_size++;
        //update allocated list
        allocatedBlock->size = chunkNewAddress+HEADER_SIZE;
        chunkNewAddress = chunkNewAddress+ chunksize;
        //to obtain the ending address of last chunk
        if(allocatedBlock->next==NULL) chunkAddress = allocatedBlock->size + List_getInt(chunkNewAddress);
        //next loop
        allocatedBlock = allocatedBlock->next;

    }
    //update freelist
    while(myalloc.freeList->next){
        struct Block* deletePointer = myalloc.freeList->next;

        List_deleteBlock(&myalloc.freeList, myalloc.freeList->next);
        free(deletePointer);
    }
    myalloc.freeList->size = chunkAddress+HEADER_SIZE;
    chunksize = myalloc.memory + myalloc.size - chunkAddress;
    int* freesize = myalloc.freeList->size - HEADER_SIZE;
    *freesize = chunksize-HEADER_SIZE;
    compacted_size++;
    pthread_mutex_unlock(&myalloc.lock);
    return compacted_size;
}

void compact_allocation_helper(void* origin,int originsize, void*destination){
    int freespace = origin - destination;
    if(freespace == 0 )return; // nothing need to be done if not moving.
    int steps = (originsize+freespace-1 )/freespace;//round up riginsize/freespace
    int leftover = originsize - (steps-1)*freespace; 
    for (int i =0;i<steps;i++){
        if(i<steps-1) memcpy(destination+i*freespace, origin+i*freespace, freespace);
        if(i==steps-1) memcpy(destination+i*freespace, origin+i*freespace, leftover);
    }
}

int available_memory() {
    pthread_mutex_lock(&myalloc.lock);
    int available_memory_size = myalloc.size;
    // Calculate available memory size

    struct Block *allocatedBlock = myalloc.allocatedList;
    while (allocatedBlock) {
        available_memory_size -= List_getInt(allocatedBlock->size - HEADER_SIZE) + HEADER_SIZE;
        allocatedBlock = allocatedBlock->next;
    }
    pthread_mutex_unlock(&myalloc.lock);
    return available_memory_size;
}

// TODO
void print_statistics() {
    pthread_mutex_lock(&myalloc.lock);
    int allocated_size = 0;
    int allocated_chunks = 0;
    int free_size = 0;
    int free_chunks = 0;
    int smallest_free_chunk_size = -1;
    int largest_free_chunk_size = 0;

    // Calculate the statistics
    // calculate allocated size and chunks
    struct Block *allocatedBlock = myalloc.allocatedList;
    while (allocatedBlock) {
        int block_size = List_getInt(allocatedBlock->size - HEADER_SIZE) + HEADER_SIZE;;
        allocated_size += block_size;
        allocated_chunks++;
        allocatedBlock = allocatedBlock->next;
    }

    // calculate free size and chunks
    struct Block *freeBlock = myalloc.freeList;
    while (freeBlock) {
        int block_size = List_getInt(freeBlock->size - HEADER_SIZE) + HEADER_SIZE;
        free_size += block_size;
        free_chunks++;

        if (smallest_free_chunk_size == -1 || block_size < smallest_free_chunk_size) {
            smallest_free_chunk_size = block_size;
        }

        if (block_size > largest_free_chunk_size) {
            largest_free_chunk_size = block_size;
        }

        freeBlock = freeBlock->next;
    }

    int total_free_memory = myalloc.size - allocated_size;
    if (total_free_memory > free_size) {
        int fragmented_free_memory = total_free_memory - free_size;
        if (fragmented_free_memory > 0 && fragmented_free_memory <= HEADER_SIZE) {
            free_chunks++;
            free_size += fragmented_free_memory;

            if (smallest_free_chunk_size == -1 || fragmented_free_memory < smallest_free_chunk_size) {
                smallest_free_chunk_size = fragmented_free_memory;
            }

            if (fragmented_free_memory > largest_free_chunk_size) {
                largest_free_chunk_size = fragmented_free_memory;
            }
        }
    }
    
    if (free_chunks == 1) {
        smallest_free_chunk_size = largest_free_chunk_size;
    }

    // set smallest_free_chunk_size to 0 if no free chunks are found
    if (smallest_free_chunk_size == -1) {
        smallest_free_chunk_size = 0;
    }


    printf("\nAllocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);

    pthread_mutex_unlock(&myalloc.lock);
}

struct BlockDetails* getFreeBlocks(int* count) {
    struct Block* current = myalloc.freeList;
    *count = 0;

    // First pass: Count the number of free blocks
    while (current) {
        (*count)++;
        current = current->next;
    }

    // Allocate an array to hold the block details
    struct BlockDetails* blocks = (struct BlockDetails*)malloc((*count) * sizeof(struct BlockDetails));

    current = myalloc.freeList;
    int index = 0;

    // Second pass: Store the block details
    while (current) {
        blocks[index].start = current->size - HEADER_SIZE;
        blocks[index].end = current->size + List_getInt(current->size - HEADER_SIZE);
        blocks[index].size = List_getInt(current->size - HEADER_SIZE) + HEADER_SIZE;
        current = current->next;
        index++;
    }

    free(current);

    return blocks;
}

struct BlockDetails* getAllocatedBlocks(int* count) {
    struct Block* current = myalloc.allocatedList;
    *count = 0;

    // First pass: Count the number of free blocks
    while (current) {
        (*count)++;
        current = current->next;
    }

    // Allocate an array to hold the block details
    struct BlockDetails* blocks = (struct BlockDetails*)malloc((*count) * sizeof(struct BlockDetails));

    current = myalloc.allocatedList;
    int index = 0;

    // Second pass: Store the block details
    while (current) {
        blocks[index].start = current->size - HEADER_SIZE;
        blocks[index].end = current->size + List_getInt(current->size - HEADER_SIZE);
        blocks[index].size = List_getInt(current->size - HEADER_SIZE) + HEADER_SIZE;
        current = current->next;
        index++;
    }

    free(current);

    return blocks;
}