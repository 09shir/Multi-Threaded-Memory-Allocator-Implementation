#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myalloc.h"
#include "list.h"

// each linked list should contain 
// 1. a pointer to the allocated memory block
// 2. a pointer to the next node in the list

struct Header {
    size_t size;
};

struct Block {
    struct Header header;
    struct Block* next;
};

struct Myalloc {
    enum allocation_algorithm aalgorithm;
    size_t size;
    void* memory;
    struct Block* freeList;
    struct Block* allocatedList;
};

struct Myalloc myalloc;

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    myalloc.aalgorithm = _aalgorithm;

    // size should align to the nearest next 64-byte boundary (round up)
    myalloc.size = (_size + 63) & ~63;
    myalloc.memory = malloc(myalloc.size);

    assert(myalloc.memory != NULL);
    memset(myalloc.memory, 0, myalloc.size);

    // initialize free block
    myalloc.freeList = (struct Block*) myalloc.memory;
    myalloc.freeList->header.size = myalloc.size;
    myalloc.freeList->next = NULL;
    myalloc.allocatedList = NULL;
}

void destroy_allocator() {
    free(myalloc.memory);
    myalloc.memory = NULL;
    // Free all nodes in freeList and allocatedList
    // Assuming List_deleteNode manages freeing nodes
    while (myalloc.freeList) {
        List_deleteNode((struct Block**)&myalloc.freeList, myalloc.freeList);
    }
    while (myalloc.allocatedList) {
        List_deleteNode((struct Block**)&myalloc.allocatedList, myalloc.allocatedList);
    }
}

void* allocate(int _size) {
    int total_size = _size + sizeof(struct Header);
    struct Block** current_block = &myalloc.freeList;
    struct Block* best_fit = NULL;
    struct Block** best_fit_prev = NULL;

    while (*current_block) {
        if ((*current_block)->header.size >= total_size) {
            if (myalloc.aalgorithm == FIRST_FIT || !best_fit || (*current_block)->header.size < best_fit->header.size) {
                best_fit = *current_block;
                best_fit_prev = current_block;
                if (myalloc.aalgorithm == FIRST_FIT) break;
            }
        }
        current_block = &(*current_block)->next;
    }

    if (!best_fit) return NULL;

    if (best_fit->header.size > total_size + sizeof(struct Block)) {
        struct Block* new_block = (struct Block*)((char*)best_fit + total_size);
        new_block->header.size = best_fit->header.size - total_size;
        new_block->next = best_fit->next;
        *best_fit_prev = new_block;  // Update the free list
    } else {
        *best_fit_prev = best_fit->next;  // Use the whole block
    }

    List_insertHead((struct Block**)&myalloc.allocatedList, best_fit);
    return (void*)((char*)best_fit + sizeof(struct Header));
}

void deallocate(void* _ptr) {
    assert(_ptr != NULL);
    struct Block* block = (struct Block*)((char*)_ptr - sizeof(struct Header));
    List_deleteNode((struct Block**)&myalloc.allocatedList, block);  // Remove from allocated list
    List_insertHead((struct Block**)&myalloc.freeList, block);  // Insert back into free list
    // Code to coalesce free blocks should be here if necessary
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



