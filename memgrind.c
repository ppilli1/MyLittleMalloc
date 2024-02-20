#include <stdio.h>
#include <stdlib.h>
#include <time.h> // For clock()
#include "mymalloc.h"

#define N 120
#define TASK_REPEATS 50

void runTask1();
void runTask2();
void runTask3();
void userTest1();
void userTest2();

int main(int argc, char **argv) {
    runTask1();
    runTask2();
    runTask3();
    userTest1();
    userTest2();
    return 0;
}

void runTask1() {
    clock_t start, end;
    start = clock();
    for (int count = 0; count < TASK_REPEATS; count++) {
        for (int i = 0; i < N; i++) {
            char *p = mymalloc(1, __FILE__, __LINE__);
            myfree(p, __FILE__, __LINE__);
        }
    }
    end = clock();
    double difference = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("Task 1: %.2f ms total, %.2f ms per run\n", difference, difference / TASK_REPEATS);
}

void runTask2() {
    clock_t start, end;
    start = clock();
    char *pointers[N];
    for (int count = 0; count < TASK_REPEATS; count++) {
        for (int i = 0; i < N; i++) {
            pointers[i] = mymalloc(1, __FILE__, __LINE__);
        }
        for (int i = 0; i < N; i++) {
            myfree(pointers[i], __FILE__, __LINE__);
        }
    }
    end = clock();
    double difference = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("Task 2: %.2f ms total, %.2f ms per run\n", difference, difference / TASK_REPEATS);
}

void runTask3() {
    clock_t start, end;
    start = clock();
    char *array[N];
    int allocated = 0;
    for (int counter = 0; counter < TASK_REPEATS; counter++) {
        allocated = 0;
        for (int i = 0; i < N; i++) {
            if (rand() % 2 == 0 && allocated < N) {
                array[allocated++] = mymalloc(1, __FILE__, __LINE__);
            } else if (allocated > 0) {
                myfree(array[--allocated], __FILE__, __LINE__);
            }
        }
        while (allocated > 0) {
            myfree(array[--allocated], __FILE__, __LINE__);
        }
    }
    end = clock();
    double difference = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("Task 3: %.2f ms total, %.2f ms per run\n", difference, difference / TASK_REPEATS);
}

void userTest1() {
    clock_t start, end;
    start = clock();
    const int maxAllocSize = 128; // Maximum block size in bytes
    const int totalMemLimit = 1024; // Total memory limit in bytes
    int totalAllocated = 0;
    char* ptrs[N]; // To keep track of allocated blocks
    int ptrCount = 0;

    while (totalAllocated < totalMemLimit && ptrCount < N) {
        int allocSize = (rand() % maxAllocSize) + 1; // Allocate between 1 and maxAllocSize bytes
        char* p = mymalloc(allocSize, __FILE__, __LINE__);
        if (p != NULL) {
            ptrs[ptrCount++] = p;
            totalAllocated += allocSize;
        }
    }

    // Free all allocated blocks
    for (int i = 0; i < ptrCount; i++) {
        myfree(ptrs[i], __FILE__, __LINE__);
    }
    end = clock();
    double difference = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("User Test 1: %.2f ms total\n", difference);
}

void userTest2() {
    clock_t start, end;
    start = clock();
    for (int count = 0; count < TASK_REPEATS; count++) {
        for (int i = 0; i < N; i++) {
            char *p = mymalloc((i % 64) + 1, __FILE__, __LINE__);
            myfree(p, __FILE__, __LINE__);
        }
    }
    end = clock();
    double difference = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("User Test 2: %.2f ms total\n", difference);
}
