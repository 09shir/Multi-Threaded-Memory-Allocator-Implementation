#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myalloc.h"
#include "list.h"
#include <stdbool.h>

// each linked list should contain 
// 1. a pointer to the allocated memory block
// 2. a pointer to the next node in the list

// struct Header {
//     size_t size;
// };

#define HEADER_SIZE 8

// struct Block {
//     // struct Header header;
//     void* size;
//     struct Block* next;
// };

struct Myalloc {
    enum allocation_algorithm aalgorithm;
    size_t size;
    void* memory; // points to head
    struct Block* freeList; //LL
    struct Block* allocatedList; //LL
};

struct Myalloc myalloc;

// void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
//     assert(_size > 0);

//     // size should align to the nearest next 64-byte boundary (round up)
//     myalloc.size = (_size + 63) & ~63;
//     myalloc.memory = malloc((size_t)myalloc.size);
//     myalloc.aalgorithm = _aalgorithm;

//     memset(myalloc.memory, 0, myalloc.size);

//     struct Block *block = List_createBlock(myalloc.memory + HEADER_SIZE);
//     // struct Block *block = List_createBlock(myalloc.memory);
//     List_insertBlock(&myalloc.freeList, block);

//     // copy size into the header
//     size_t header_size = myalloc.size - HEADER_SIZE;
//     memcpy(myalloc.memory, &header_size, HEADER_SIZE);
//     myalloc.allocatedList = NULL;
// }

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);

    // size should align to the nearest next 64-byte boundary (round up)
    myalloc.size = (_size + 63) & ~63;
    // myalloc.size = _size;
    myalloc.memory = malloc(myalloc.size);
    myalloc.aalgorithm = _aalgorithm;

    memset(myalloc.memory, 0, myalloc.size);

    struct Block *block = List_createBlock(myalloc.memory + HEADER_SIZE);
    // printf("%zu\n", *(size_t*) block->size);
    // struct memoryBlock *block = List_createBlock(myalloc.memory);
    List_insertBlock(&myalloc.freeList, block);

    // copy size into the header
    size_t header_size = myalloc.size - HEADER_SIZE;
    memcpy(myalloc.memory, &header_size, HEADER_SIZE);
    myalloc.allocatedList = NULL;

    printf("Initialized header of myalloc.memory: %zu\n", *(size_t*)myalloc.memory);
}

void destroy_allocator() {
    free(myalloc.memory);
    myalloc.memory = NULL;
    // Free all nodes in freeList and allocatedList
    // Assuming List_deleteNode manages freeing nodes

}

// TODO: Best Fit
void* allocate(int _size) {
    void* ptr = NULL;

    // Allocate memory from myalloc.memory 
    // ptr = address of allocated memory

    int total_size = _size + HEADER_SIZE;
    struct Block *curr = myalloc.freeList;
    struct Block *tmp = NULL;
    struct Block *best_fit = NULL;

    if (myalloc.aalgorithm == FIRST_FIT) {
        while (curr) {
            // if (List_getSize(curr->size) > total_size) {
            if (List_getInt(curr->size - HEADER_SIZE) >= total_size) {
                best_fit = curr;
                break;
            }
            curr = curr->next;
        }
    }
    else if (myalloc.aalgorithm == BEST_FIT) {
        while (curr) {
            // TO DO
            curr = curr->next;
            tmp = tmp->next;
        }
    }

    // printf("\n    best_fit: %p\n", best_fit);
    // printf("    free size (best_fit->size): %d\n", List_getInt(best_fit->size - HEADER_SIZE));
    // printf("    requested size + HEADER_SIZE: %d\n", _size + HEADER_SIZE);

    // a new header will only be added if (free size (header not included) > requested size + HEADER_SIZE)
    if (List_getInt(best_fit->size - HEADER_SIZE) > _size + HEADER_SIZE) {
        // add new header
        size_t header_size = List_getSize_t(best_fit->size - HEADER_SIZE) - _size - HEADER_SIZE;
        memcpy(best_fit->size + _size, &header_size, HEADER_SIZE);

        // push new free block from below
        struct Block* newBlock = List_createBlock(best_fit->size + _size + HEADER_SIZE);
        // printf(" \ninserting block memory: %d\n", List_getInt(newBlock->size - HEADER_SIZE));
        List_insertBlock(&myalloc.freeList, newBlock);
    }

    // change old header
    // printf("1 ");
    *(size_t *) (best_fit->size - HEADER_SIZE) = _size;

    // remove it from freeList
    // printf("2 ");
    List_deleteBlock(&myalloc.freeList, best_fit);
    // add it to allocatedList
    // printf("3 ");
    List_insertBlock(&myalloc.allocatedList, best_fit);
    // printf("4 \n");

    ptr = best_fit->size;

    // check if allocatedList has best_fit
    // struct Block *b = myalloc.allocatedList;
    // printf("Pushed Block with best_fit->size: %d to allocatedList\n", List_getInt(b->size - HEADER_SIZE));

    return ptr;
}

void deallocate(void* _ptr) {
    assert(_ptr != NULL);

    struct Block* block_to_remove = List_searchBlock(myalloc.allocatedList, _ptr);

    List_deleteBlock(&myalloc.allocatedList, block_to_remove);
    List_insertBlock(&myalloc.freeList, block_to_remove);

    printf("deallocated %p\n", block_to_remove->size);

    printf("available_memory %d\n", available_memory());

    // go through all free blocks, if next free block is consecutive, merge
    // by checking if there's any allocated block in between the two free blocks

    // printf("0 ");
    // struct Block* freeBlock = myalloc.freeList;
    // struct Block* allocatedBlock = myalloc.allocatedList;
    // struct Block* tmpAllocatedBlock = allocatedBlock;
    // bool merge = true;
    // printf("1 ");
    // while (freeBlock->next) {
    //     while (tmpAllocatedBlock) {
    //         if (tmpAllocatedBlock->size > freeBlock->size && tmpAllocatedBlock->size > freeBlock->next->size){
    //             merge = false;
    //             break;
    //         }
    //     }
    //     if (merge) {
    //         // edit add freeBlock->next size to freeBlock
    //         size_t header_size = List_getSize_t(freeBlock->size - HEADER_SIZE) + HEADER_SIZE + List_getSize_t(freeBlock->next->size - HEADER_SIZE);
    //         memcpy(freeBlock->size, &header_size, HEADER_SIZE);

    //         // remove freeBlock->next from freeList
    //         List_deleteBlock(&myalloc.freeList, freeBlock->next);
    //     }
    //     tmpAllocatedBlock = allocatedBlock;
    // }
    // printf("2\n");
    
}

// TODO
int compact_allocation(void** _before, void** _after) {
    int compacted_size = 0;

    // compact allocated memory
    // update _before, _after and compacted_size

    return compacted_size;
}

int available_memory() {
    int available_memory_size = myalloc.size;
    // Calculate available memory size

    struct Block *allocatedBlock = myalloc.allocatedList;
    while (allocatedBlock) {
        available_memory_size -= List_getInt(allocatedBlock->size - HEADER_SIZE) + HEADER_SIZE;
        allocatedBlock = allocatedBlock->next;
    }
    return available_memory_size;
}

// TODO
void print_statistics() {
    int allocated_size = 0;
    int allocated_chunks = 0;
    int free_size = 0;
    int free_chunks = 0;
    int smallest_free_chunk_size = 0;
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
}



