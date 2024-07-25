#include <stdio.h>
#include "myalloc.h"
//#include "myalloc.c"//don't delete for william debug

// int TOTAL_MEMORY_SIZE = 100;
// int EACH_MEMORY_SIZE = sizeof(int);

int SUCCESS_CASES = 0;
int TOTAL_CASES = 0;

#define TEST(f) do { \
    if (f) { \
        SUCCESS_CASES += 1; \
    } else { \
        printf("Test #%d failed: %s at %s: %d\n", TOTAL_CASE+1, #f, __FILE__, __LINE__); \
    } \
    TOTAL_CASE += 1; \
} while (0)

void allocation_test_1() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(i+1);
        TEST()
    }

}

int main(int argc, char* argv[]) {
    // printf("size of each allocated memory size: %d\n", EACH_MEMORY_SIZE);
    
    initialize_allocator(100, FIRST_FIT);
    // initialize_allocator(100, BEST_FIT);
    // initialize_allocator(100, WORST_FIT);

    return 0;
}
