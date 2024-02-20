#include <stdio.h>
#include <string.h>
#include "mymalloc.h"

#define LARGE_OBJECT_SIZE 1024
#define NUM_LARGE_OBJECTS 5

void testNonOverlappingAllocation();
void testFreeAndCoalescing();
void testErrorDetection();

int main() {
    testNonOverlappingAllocation();
    testFreeAndCoalescing();
    testErrorDetection();
    return 0;
}

void testNonOverlappingAllocation() {
    printf("Test 1: Non-overlapping Allocation and Pattern Integrity\n");

    char* ptrs[NUM_LARGE_OBJECTS];
    for (int i = 0; i < NUM_LARGE_OBJECTS; i++) {
        ptrs[i] = mymalloc(LARGE_OBJECT_SIZE, __FILE__, __LINE__);
        if (ptrs[i] == NULL) {
            printf("  Allocation failed for object %d\n", i);
            return;
        }
        // Fill each object with a distinct pattern
        memset(ptrs[i], i + 1, LARGE_OBJECT_SIZE);
    }

    // Verify integrity of each object
    for (int i = 0; i < NUM_LARGE_OBJECTS; i++) {
        for (int j = 0; j < LARGE_OBJECT_SIZE; j++) {
            if (ptrs[i][j] != i + 1) {
                printf("  Pattern integrity check failed for object %d\n", i);
                return;
            }
        }
    }

    printf("  All objects maintain distinct patterns. Test passed.\n");
}

void testFreeAndCoalescing() {
    printf("\nTest 2: Free and Coalescing of Adjacent Free Blocks\n");

    // Allocate two blocks
    char *block1 = mymalloc(LARGE_OBJECT_SIZE / 2, __FILE__, __LINE__);
    char *block2 = mymalloc(LARGE_OBJECT_SIZE / 2, __FILE__, __LINE__);

    // Free the first block
    myfree(block1, __FILE__, __LINE__);

    // Allocate a block that requires coalescing to fit
    char *block3 = mymalloc(LARGE_OBJECT_SIZE, __FILE__, __LINE__);
    if (block3 != NULL) {
        printf("  Coalescing appears to work. Test passed.\n");
    } else {
        printf("  Failed to allocate after free, suggesting coalescing did not occur.\n");
    }

    // Clean up
    myfree(block2, __FILE__, __LINE__);
    myfree(block3, __FILE__, __LINE__);
}

void testErrorDetection() {
    printf("\nTest 3: Error Detection and Reporting\n");

    // Attempt to free a pointer not returned by malloc
    int notAllocated;
    myfree(&notAllocated, __FILE__, __LINE__);

    // Double free attempt
    char *ptr = mymalloc(1, __FILE__, __LINE__);
    myfree(ptr, __FILE__, __LINE__);
    myfree(ptr, __FILE__, __LINE__); // This should trigger an error

    // Attempt to allocate zero bytes
    mymalloc(0, __FILE__, __LINE__);

    printf("  If errors were reported above, error detection works.\n");
}
