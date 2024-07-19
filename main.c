#include <stdio.h>
#include "myalloc.h"

int TOTAL_MEMORY_SIZE = 100;
int EACH_MEMORY_SIZE = sizeof(int);

int main(int argc, char* argv[]) {
    // printf("size of each allocated memory size: %d\n", EACH_MEMORY_SIZE);
    
    initialize_allocator(TOTAL_MEMORY_SIZE, FIRST_FIT);
    // initialize_allocator(100, BEST_FIT);
    // initialize_allocator(100, WORST_FIT);

    printf("Using first fit algorithm on memory size %d \n", TOTAL_MEMORY_SIZE);

    int* p[50] = {NULL};
    for(int i=0; i<10; ++i) {
        p[i] = allocate(EACH_MEMORY_SIZE);
        if(p[i] == NULL) {
            printf("%d: Allocation failed\n", i);
            continue;
        }
        *(p[i]) = i;
        printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
    }

    print_statistics();

    for (int i = 0; i < 10; ++i) {
        if (i % 2 == 0)
            continue;

        printf("Freeing p[%d]\n", i);
        deallocate(p[i]);
        p[i] = NULL;
    }

    printf("available_memory %d", available_memory());

    // void* before[100] = {NULL};
    // void* after[100] = {NULL};
    // compact_allocation(before, after);

    // print_statistics();

    // You can assume that the destroy_allocator will always be the 
    // last funciton call of main function to avoid memory leak 
    // before exit

    // destroy_allocator();

    return 0;
}
