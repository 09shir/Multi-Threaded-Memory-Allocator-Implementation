#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "myalloc.h"

struct Myalloc {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    // Some other data members you want, 
    // such as lists to record allocated/free memory
};

struct Myalloc myalloc;

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    myalloc.aalgorithm = _aalgorithm;
    myalloc.size = _size;
    myalloc.memory = malloc((size_t)myalloc.size);

    // Add some other initialization 
}

void destroy_allocator() {
    free(myalloc.memory);

    // free other dynamic allocated memory to avoid memory leak
}

void* allocate(int _size) {
    void* ptr = NULL;

    // Allocate memory from myalloc.memory 
    // ptr = address of allocated memory

    return ptr;
}

void deallocate(void* _ptr) {
    assert(_ptr != NULL);

    // Free allocated memory
    // Note: _ptr points to the user-visible memory. The size information is
    // stored at (char*)_ptr - 8.
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



