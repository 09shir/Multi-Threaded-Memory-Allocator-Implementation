#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myalloc.h"
#include "list.h"

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
    void* memory;
    struct Block* freeList;
    struct Block* allocatedList;
};

struct Myalloc myalloc;

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);

    // size should align to the nearest next 64-byte boundary (round up)
    myalloc.size = (_size + 63) & ~63;
    myalloc.memory = malloc((size_t)myalloc.size);
    myalloc.aalgorithm = _aalgorithm;

    memset(myalloc.memory, 0, myalloc.size);

    struct Block *block = List_createBlock(myalloc.memory + HEADER_SIZE);
    // struct Block *block = List_createBlock(myalloc.memory);
    List_insertBlock(&myalloc.freeList, block);

    // copy size into the header
    size_t header_size = myalloc.size - HEADER_SIZE;
    memcpy(myalloc.memory, &header_size, HEADER_SIZE);
    myalloc.allocatedList = NULL;
}

void destroy_allocator() {
    free(myalloc.memory);
    myalloc.memory = NULL;
    // Free all nodes in freeList and allocatedList
    // Assuming List_deleteNode manages freeing nodes

}

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
            if (*(size_t *)curr->size > total_size) {
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

    // a new header will only be added if (free size (header not included) > requested size + HEADER_SIZE)
    if (*(size_t *)(best_fit->size - HEADER_SIZE) > _size + HEADER_SIZE) {
        // add new header
        size_t header_size = *(size_t *) (best_fit->size - HEADER_SIZE - _size - HEADER_SIZE);
        memcpy(best_fit->size + _size + HEADER_SIZE, &header_size, HEADER_SIZE);
    }

    // change old header
    // size_t header_size = _size;
    // memcpy(best_fit->size, header_size, HEADER_SIZE);
    *(size_t *) best_fit->size = _size;

    // remove it from freeList
    List_deleteBlock(&myalloc.freeList, best_fit);
    // add it to allocatedList
    List_insertBlock(&myalloc.allocatedList, best_fit);

    ptr = best_fit->size;

    return ptr;
}

void deallocate(void* _ptr) {
    assert(_ptr != NULL);
    // struct Block* block = (struct Block*)((char*)_ptr - sizeof(struct Header));
    // List_deleteNode((struct Block**)&myalloc.allocatedList, block);  // Remove from allocated list
    // List_insertHead((struct Block**)&myalloc.freeList, block);  // Insert back into free list
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



