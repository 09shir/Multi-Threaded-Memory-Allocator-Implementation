#include <stdio.h>
#include "myalloc.h"
#include "stdbool.h"
#include <stdlib.h>
//#include "myalloc.c"//don't delete for william debug

// int TOTAL_MEMORY_SIZE = 100;
// int EACH_MEMORY_SIZE = sizeof(int);

#define HEADER_SIZE 8

int SUCCESS_CASES = 0;
int TOTAL_CASES = 0;

#define TEST(f) do { \
  if (f) { \
    SUCCESS_CASES += 1; \
  } else { \
    printf("Test #%d failed: %s at %s:%d\n", TOTAL_CASES+1, #f, __FILE__, __LINE__); \
  } \
  TOTAL_CASES += 1; \
} while (0)

void print_test_result() {
  printf("----TEST_RESULT_Success: %d/%d\n", SUCCESS_CASES, TOTAL_CASES);
}

bool compare(void* a, int b) {
    return (*(int *)((char *)a - HEADER_SIZE) == b);
}

void allocation_test_1() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 9; i++) {
        p[i] = allocate(i+1);
        TEST(compare(p[i], i+1));
    }

    p[9] = allocate(10);
    TEST(p[9] == NULL);

    destroy_allocator();
}

void allocation_test_2() {
    initialize_allocator(100, FIRST_FIT);

    int *p = NULL;
    p = allocate(120);
    TEST(p != NULL);
    TEST(compare(p, 120));

    destroy_allocator();
}

void allocation_test_3() {
    initialize_allocator(100, FIRST_FIT);

    int *p = NULL;
    p = allocate(121);
    TEST(p == NULL);

    destroy_allocator();
}

void allocation_test_4() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[1]);
    deallocate(p[2]);
    deallocate(p[4]);

    p[10] = allocate(11);
    TEST(p[10] == p[1]);

    destroy_allocator();
}

void allocation_test_5() {
    initialize_allocator(100, BEST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[1]);
    deallocate(p[2]);
    deallocate(p[4]);

    p[10] = allocate(4);
    TEST(p[10] == p[4]);

    destroy_allocator();
}

void allocation_test_6() {
    initialize_allocator(100, BEST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[1]);
    deallocate(p[2]);
    deallocate(p[3]);
    deallocate(p[5]);
    deallocate(p[6]);

    p[10] = allocate(11);

    TEST(p[10] == p[5]);

    destroy_allocator();
}

void allocation_test_7() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    p[0] = allocate(9);
    p[1] = allocate(9);
    p[2] = allocate(8);
    p[3] = allocate(8);
    p[4] = allocate(7);
    p[5] = allocate(7);
    p[6] = allocate(6);
    p[7] = allocate(6);

    deallocate(p[0]);
    deallocate(p[2]);
    deallocate(p[4]);
    deallocate(p[6]);

    p[8] = allocate(6);

    TEST(p[8] == p[0]);

    destroy_allocator();
}

void allocation_test_8() {
    initialize_allocator(100, BEST_FIT);

    int *p[11] = {NULL};

    p[0] = allocate(9);
    p[1] = allocate(9);
    p[2] = allocate(8);
    p[3] = allocate(8);
    p[4] = allocate(7);
    p[5] = allocate(7);
    p[6] = allocate(6);
    p[7] = allocate(6);

    deallocate(p[0]);
    deallocate(p[2]);
    deallocate(p[4]);
    deallocate(p[6]);

    p[8] = allocate(6);

    TEST(p[8] == p[6]);

    destroy_allocator();
}

void deallocate_test_1() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[1]);
    deallocate(p[2]);
    deallocate(p[3]);

    int count;
    struct BlockDetails* blocks = getFreeBlocks(&count);

    TEST(blocks[0].size == 36);
    TEST(count == 1);

    free(blocks);

    destroy_allocator();
}

void deallocate_test_2() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[0]);
    deallocate(p[1]);
    deallocate(p[2]);
    deallocate(p[3]);
    deallocate(p[4]);
    deallocate(p[5]);
    deallocate(p[6]);
    deallocate(p[7]);
    deallocate(p[8]);
    deallocate(p[9]);

    int allocatedBlocksCount;
    struct BlockDetails* freeBlocks = getFreeBlocks(&allocatedBlocksCount);
    TEST(freeBlocks[0].size == 128);

    struct BlockDetails* allocatedBlocks = getAllocatedBlocks(&allocatedBlocksCount);
    TEST(allocatedBlocks != NULL);
    TEST(allocatedBlocksCount == 0);

    free(freeBlocks);
    free(allocatedBlocks);

    destroy_allocator();
}

void deallocate_test_3() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[1]);
    deallocate(p[3]);
    deallocate(p[5]);
    deallocate(p[7]);
    deallocate(p[9]);

    int freeBlocksCount;
    int allocatedBlocksCount;
    struct BlockDetails* freeBlocks = getFreeBlocks(&freeBlocksCount);
    struct BlockDetails* allocatedBlocks = getAllocatedBlocks(&allocatedBlocksCount);

    TEST(freeBlocksCount == 5);
    TEST(allocatedBlocksCount == 5);

    for (int i = 0; i < 4; i++) {
        TEST(freeBlocks[i].size == 12);
        TEST(allocatedBlocks[i].size == 12);
    }

    TEST(freeBlocks[4].size == 20);
    TEST(allocatedBlocks[4].size == 12);

    free(freeBlocks);
    free(allocatedBlocks);

    destroy_allocator();
}

void deallocate_test_4() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[9]);
    deallocate(p[7]);
    deallocate(p[5]);
    deallocate(p[3]);
    deallocate(p[1]);

    int freeBlocksCount;
    int allocatedBlocksCount;
    struct BlockDetails* freeBlocks = getFreeBlocks(&freeBlocksCount);
    struct BlockDetails* allocatedBlocks = getAllocatedBlocks(&allocatedBlocksCount);

    TEST(freeBlocksCount == 5);
    TEST(allocatedBlocksCount == 5);

    for (int i = 0; i < 4; i++) {
        TEST(freeBlocks[i].size == 12);
        TEST(allocatedBlocks[i].size == 12);
    }

    TEST(freeBlocks[4].size == 20);
    TEST(allocatedBlocks[4].size == 12);

    free(freeBlocks);
    free(allocatedBlocks);

    destroy_allocator();
}

void deallocate_test_5() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[9]);

    int freeBlocksCount;
    int allocatedBlocksCount;
    struct BlockDetails* freeBlocks = getFreeBlocks(&freeBlocksCount);
    struct BlockDetails* allocatedBlocks = getAllocatedBlocks(&allocatedBlocksCount);

    TEST(freeBlocksCount == 1);
    TEST(allocatedBlocksCount == 9);

    TEST(freeBlocks[0].size == 20);
    TEST(allocatedBlocks[1].size == 12);

    free(freeBlocks);
    free(allocatedBlocks);

    destroy_allocator();
}

void deallocate_test_6() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[3]);
    deallocate(p[2]);
    deallocate(p[1]);

    int freeBlocksCount;
    int allocatedBlocksCount;
    struct BlockDetails* freeBlocks = getFreeBlocks(&freeBlocksCount);
    TEST(freeBlocksCount == 1);
    TEST(freeBlocks[0].size == 36);

    struct BlockDetails* allocatedBlocks = getAllocatedBlocks(&allocatedBlocksCount);
    TEST(allocatedBlocks[0].size == 12);
    TEST(allocatedBlocksCount == 7);

    free(freeBlocks);
    free(allocatedBlocks);
    destroy_allocator();
}

void compact_allocation_test_1() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[1]);
    deallocate(p[2]);
    deallocate(p[4]);
    deallocate(p[5]);

    void* before[10];
    void* after[10];

    int compacted_size = compact_allocation(before, after);
    TEST(compacted_size == 7);

    int freeBlocksCount;
    int allocatedBlocksCount;
    struct BlockDetails* freeBlocks = getFreeBlocks(&freeBlocksCount);
    TEST(freeBlocksCount == 1);
    TEST(freeBlocks[0].size == 56);

    struct BlockDetails* allocatedBlocks = getAllocatedBlocks(&allocatedBlocksCount);
    TEST(allocatedBlocksCount == 6);
    TEST(allocatedBlocks[0].size == 12);

    free(freeBlocks);
    free(allocatedBlocks);

    destroy_allocator();
}

void compact_allocation_test_2() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[0]);
    deallocate(p[2]);
    deallocate(p[4]);
    deallocate(p[6]);
    deallocate(p[8]);

    void* before[10];
    void* after[10];

    int compacted_size = compact_allocation(before, after);
    TEST(compacted_size == 6);

    int freeBlocksCount;
    int allocatedBlocksCount;
    struct BlockDetails* freeBlocks = getFreeBlocks(&freeBlocksCount);
    TEST(freeBlocksCount == 1);
    TEST(freeBlocks[0].size == 68);

    struct BlockDetails* allocatedBlocks = getAllocatedBlocks(&allocatedBlocksCount);
    TEST(allocatedBlocks[0].size == 12);
    TEST(allocatedBlocksCount == 5);

    free(freeBlocks);
    free(allocatedBlocks);

    destroy_allocator();
}

void compact_allocation_test_3() {
    initialize_allocator(100, FIRST_FIT);

    int *p[11] = {NULL};

    for (int i = 0; i < 10; i++) {
        p[i] = allocate(4);
    }

    deallocate(p[1]);
    deallocate(p[3]);
    deallocate(p[5]);
    deallocate(p[7]);
    deallocate(p[9]);

    void* before[10];
    void* after[10];

    int compacted_size = compact_allocation(before, after);
    TEST(compacted_size == 6);

    int freeBlocksCount;
    int allocatedBlocksCount;
    struct BlockDetails* freeBlocks = getFreeBlocks(&freeBlocksCount);
    TEST(freeBlocksCount == 1);
    TEST(freeBlocks[0].size == 68);
    struct BlockDetails* allocatedBlocks = getAllocatedBlocks(&allocatedBlocksCount);
    TEST(allocatedBlocks[0].size == 12);
    TEST(allocatedBlocksCount == 5);

    free(freeBlocks);
    free(allocatedBlocks);

    destroy_allocator();
}

int main(int argc, char* argv[]) {

    allocation_test_1();
    allocation_test_2();
    allocation_test_3();
    allocation_test_4();
    allocation_test_5();
    allocation_test_6();
    allocation_test_7();
    allocation_test_8();

    deallocate_test_1();
    deallocate_test_2();
    deallocate_test_3();
    deallocate_test_4();
    deallocate_test_5();
    deallocate_test_6();
    compact_allocation_test_1();
    compact_allocation_test_2();
    compact_allocation_test_3();
      
    print_test_result();
    return 0;
}