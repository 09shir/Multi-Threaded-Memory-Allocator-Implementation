#include "myalloc.h"
#include <memory.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int SUCCESS_CAES = 0;
int TOTAL_CASE = 0;

#define TEST(f) do { \
  if (f) { \
    SUCCESS_CAES += 1; \
  } else { \
    printf("Test #%d failed: %s at %s:%d\n", TOTAL_CASE+1, #f, __FILE__, __LINE__); \
  } \
  TOTAL_CASE += 1; \
} while (0)

void print_test_result() {
  printf("----TEST_RESULT_Success: %d/%d\n", SUCCESS_CAES, TOTAL_CASE);
}

#define HEADER_SIZE 8

void test_correct_space_allocation() {
  // Test Allocation Not Overwrite
  void *p[10] = {NULL};
  for (int i = 0; i < 10; i++) {
    __int32_t *ptr = (__int32_t *)allocate(sizeof(__int32_t) * 2);
    printf("Allocating %d: %p, %lu\n", i, ptr, sizeof(ptr));

    if (ptr != NULL) {
      ptr[0] = 0;
      ptr[1] = 1;
      // Inspect header
      if (i < 5) {
        // printf("1\n");
        TEST(*(uint64_t *)((char *)ptr - HEADER_SIZE) ==
                 (sizeof(__int32_t) * 2) + HEADER_SIZE ||
             *(uint64_t *)((char *)ptr - HEADER_SIZE) ==
                 (sizeof(__int32_t) * 2));
      }
    }
    p[i] = ptr;
  }
  for (int i = 0; i < 10; i++) {
    if (i < 6) {
    //   printf("2\n");
      TEST(p[i] != NULL);
    } else {
    //   printf("3: %d, %p\n", i, p[i]);
      TEST(p[i] == NULL);
    }
  }
  TEST(*((uint64_t *)((char *)p[5] - HEADER_SIZE)) == 20);
  printf("> Result: %llu\n", *((uint64_t *)((char *)p[5] - HEADER_SIZE)));
  TEST(available_memory() == 0);
  printf("> available_memory() == %d\n", available_memory());
  for (int i = 0; i < 10; i++) {
    __int32_t *ptr = (__int32_t *)p[i];
    if (ptr != NULL) {
      TEST(ptr[0] == 0);
      TEST(ptr[1] == 1);
    }
  }

  for (int i = 0; i < 10; i++) {
    if (p[i] != NULL) {
      deallocate(p[i]);
      p[i] = NULL;
    }
  }
  TEST(available_memory() == 92);
  printf("> available_memory() == %d\n", available_memory());
  struct Stats stats;
  get_statistics(&stats);
  TEST(stats.allocated_size == 0);
  TEST(stats.allocated_chunks == 0);
  TEST(stats.free_size == 92);
  printf("> free_size == %d\n", stats.free_size);
  TEST(stats.free_chunks == 1);
  printf("> free_chunks == %d\n", stats.free_chunks);
  TEST(stats.largest_free_chunk_size == 92);
  printf("> largest_free_chunk_size == %d\n", stats.largest_free_chunk_size);
  TEST(stats.smallest_free_chunk_size == 92);
  printf("> smallest_free_chunk_size == %d\n", stats.smallest_free_chunk_size);
}

void test_compact() {
  void *p[10] = {NULL};
  void *before[10] = {NULL};
  void *after[10] = {NULL};

  for (int i = 0; i < 8; i++) {
    p[i] = allocate(4);
  }
  deallocate(p[1]);
  deallocate(p[3]);
  TEST(allocate(8) == NULL);

  compact_allocation(before, after);
  TEST(allocate(8) != NULL);
}

void test_first_fit_algorithm() {
  void *p[10] = {NULL};
  for (int i = 0; i < 10; i++) {
    p[i] = allocate(4);
  }
  for (int i = 0; i < 5; i++) {
    unsigned long j = (char *)p[i + 1] - (char *)p[i];
    TEST(j == 12);
  }
  struct Stats stats;
  get_statistics(&stats);

  deallocate(p[1]);
  deallocate(p[3]);
  deallocate(p[5]);

  TEST(allocate(3) == p[1]);
  TEST(allocate(3) == p[3]);
  TEST(allocate(3) == p[5]);
}

void *thread_allocate(void *arg) {
  void **p = (void **)arg;
  *p = allocate(5);
  pthread_exit(NULL);
}

void *thread_deallocate(void *arg) {
  void **p = (void **)arg;
  deallocate(*p);
  pthread_exit(NULL);
}

void test_threading() {
  void *p[10] = {NULL};
  pthread_t tid[10];

  for (int i = 0; i < 10; i++) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid[i], &attr, thread_allocate, &p[i]);
    pthread_attr_destroy(&attr);
  }

  for (int i = 0; i < 10; i++) {
    pthread_join(tid[i], NULL);
  }

  struct Stats stats;
  get_statistics(&stats);
  TEST(stats.allocated_size == 35);
  TEST(stats.allocated_chunks == 7);
  TEST(stats.free_size == 1);
  TEST(stats.free_chunks == 1);
  TEST(stats.smallest_free_chunk_size == 1);
  TEST(stats.largest_free_chunk_size == 1);

  int non_nulls[10] = {0};
  int cnt = 0;
  for (int i = 0; i < 10; i++) {
    if (p[i] != NULL) {
      non_nulls[cnt++] = i;
    }
  }

  for (int i = 0; i < 3; i++) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid[i], &attr, thread_deallocate, &p[non_nulls[i * 2 + 1]]);
    pthread_attr_destroy(&attr);
  }

  for (int i = 0; i < 3; i++) {
    pthread_join(tid[i], NULL);
  }

  get_statistics(&stats);
  TEST(stats.allocated_size == 20);
  TEST(stats.allocated_chunks == 4);
}

void test_first_fit(int i) {
  initialize_allocator(100, FIRST_FIT);
  if (i == 0) {
    test_correct_space_allocation();
  } else if (i == 1) {
    test_first_fit_algorithm();
  } else if (i == 2) {
    test_compact();
  } else if (i == 3) {
    test_threading();
  }

  destroy_allocator();
}

void test_best_fit_algorithm() {
  void *p[10] = {NULL};
  for (int i = 0; i < 10; i++) {
    p[i] = allocate(4);
  }
  for (int i = 0; i < 5; i++) {
    unsigned long j = (char *)p[i + 1] - (char *)p[i];
    TEST(j == 12);
  }

  deallocate(p[0]);
  deallocate(p[2]);
  deallocate(p[3]);

  TEST(allocate(4) == p[0]);
}

void test_best_fit(int i) {
  initialize_allocator(100, BEST_FIT);

  if (i == 0) {
    test_correct_space_allocation();
  } else if (i == 1) {
    test_best_fit_algorithm();
  } else if (i == 2) {
    test_compact();
  } else if (i == 3) {
    test_threading();
  }

  destroy_allocator();
}

void test_worst_fit_algorithm() {
  void *p[10] = {NULL};
  for (int i = 0; i < 10; i++) {
    p[i] = allocate(4);
  }
  for (int i = 0; i < 5; i++) {
    unsigned long j = (char *)p[i + 1] - (char *)p[i];
    TEST(j == 12);
  }

  deallocate(p[0]);
  deallocate(p[2]);
  deallocate(p[3]);

  TEST(allocate(4) == p[2]);
}

void test_worst_fit(int i) {
  initialize_allocator(100, WORST_FIT);
  if (i == 0) {
    test_correct_space_allocation();
  } else if (i == 1) {
    test_worst_fit_algorithm();
  } else if (i == 2) {
    test_compact();
  } else if (i == 3) {
    test_threading();
  }

  destroy_allocator();
}

/**
 * arg 1: caseIndex
 *       -> 0: test_first_fit
 *       -> 1: test_best_fit
 * arg 2: testIndex
 *       -> 0: test_correct_space_allocation()
 *       -> 1: test_first_fit_algorithm()
 *       -> 2: test_compact() (not done)
 *       -> 3: test_threading()
 */
int main(int argc, char *argv[]) {
  if (argc != 3) {
    for (int i = 0; i < 4; i++) {
      test_first_fit(i);
      test_best_fit(i);
    }
  } else {
    int caseIndex = atoi(argv[1]);
    int testIndex = atoi(argv[2]);
    if (caseIndex == 0) {
      test_first_fit(testIndex);
    } else if (caseIndex == 1) {
      test_best_fit(testIndex);
    } 
  }
  print_test_result();

  return 0;
}