#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myalloc.h"

// each linked list should contain 
// 1. a pointer to the allocated memory block
// 2. a pointer to the next node in the list

struct Header {
    size_t size;
};

struct Block {
    struct Header *header;
    size_t size;
};

struct List_Block {
    // size_t size;
    struct List_Block* curr;
    struct List_Block* next;
};

struct Myalloc {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    struct List_Block *freeList;
    struct List_Block *allocatedList;
};

struct Myalloc myalloc;

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    myalloc.aalgorithm = _aalgorithm;

    // size should align to the nearest next 64-byte boundary (round up)
    myalloc.size = (_size + 63) & ~63;
    myalloc.memory = malloc((size_t)myalloc.size);

    assert(myalloc.memory != NULL);
    memset(myalloc.memory, 0, myalloc.size);

    // initialize free block
    myalloc.freeList = (struct List_Block*)myalloc.memory;
    myalloc.freeList->size = myalloc.size;
    myalloc.freeList->next = NULL;
    myalloc.allocatedList = NULL;
}

void destroy_allocator() {
    myalloc.memory = NULL;
    myalloc.freeList = NULL;
    myalloc.allocatedList = NULL;
    free(myalloc.memory);

    // free other dynamic allocated memory to avoid memory leak
}

void* allocate(int _size) {
    // int total_size = (_size + sizeof(int) + 63) & ~63;
    int total_size = _size + sizeof(struct Header); // Header: 8 bytes from size_t
    struct Block** best_fit_block = NULL;
    struct Block** current_block = &myalloc.freeList;

    if (myalloc.aalgorithm == FIRST_FIT) {
        while (*current_block != NULL){
            if ((*current_block)->size >= total_size) {
                best_fit_block = current_block;
                break;
            }
            current_block = &((*current_block).header->next);
        }
    }
    else if (myalloc.aalgorithm == BEST_FIT) {
        while (*current_block != NULL){
            if (best_fit_block == NULL || (*current_block)->size < (*best_fit_block)->size) {
                best_fit_block = current_block;
            }
            current_block = &((*current_block).header->next);
        }
    } 

    if (best_fit_block == NULL) {
        return NULL;
    }

    struct Block* tmp_block = *best_fit_block;
    if (tmp_block->size > total_size + sizeof(struct Block)) {
        printf("block size (%zu) is bigger than total_size %lu \n", tmp_block->size, total_size + sizeof(struct Block));
        struct Block* new_block = (struct Block*)((char*)tmp_block + total_size);
        new_block->size = tmp_block->size - total_size;
        new_block.header->next = tmp_block.header->next;
        tmp_block.header->next = new_block;
        tmp_block->size = total_size;
    }
    else {
        printf("block size (%zu) is smaller than total_size %lu \n", tmp_block->size, total_size + sizeof(struct Block));
    }

    *best_fit_block = tmp_block.header->next;
    tmp_block.header->next = myalloc.allocatedList;
    myalloc.allocatedList = tmp_block;

    return (void*)((char*)tmp_block + sizeof(int));


    // void* ptr = NULL;

    // Allocate memory from myalloc.memory 
    // ptr = address of allocated memory

    // return ptr;
}

void deallocate(void* _ptr) {
    assert(_ptr != NULL);

    // Free allocated memory
    // Note: _ptr points to the user-visible memory. The size information is
    // stored at (char*)_ptr - 8.

    struct Block* block = (struct Block*)((char*)_ptr - sizeof(int));
    struct Block** current_block = &myalloc.allocatedList;

    while (*current_block != NULL) {
        if (*current_block == block) {
            *current_block = block.header->next;
            break;
        }
        current_block = &((*current_block).header->next);
    }

    block.header->next = myalloc.freeList;
    myalloc.freeList = block;

    // Combine contiguous free blocks
    struct Block* current = myalloc.freeList;
    while (current != NULL && current.header->next != NULL) {
        if ((char*)current + current->size == (char*)current.header->next) {
            current->size += current.header->next->size;
            current.header->next = current.header->next->next;
        } else {
            current = current.header->next;
        }
    }
}

int compact_allocation(void** _before, void** _after) {
    int compacted_size = 0;

    // compact allocated memory
    // update _before, _after and compacted_size

    return compacted_size;
}

int available_memory() {
    int available_memory_size = 0;
    // Calculate available memory size
    return available_memory_size;
}

void print_statistics() {
    int allocated_size = 0;
    int allocated_chunks = 0;
    int free_size = 0;
    int free_chunks = 0;
    int smallest_free_chunk_size = myalloc.size;
    int largest_free_chunk_size = 0;

    // Calculate the statistics

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
}



