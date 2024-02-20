#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Define the memory length as per assignment specification
#define MEMLENGTH 512

// Use a double array for memory to ensure 8-byte alignment
static double memory[MEMLENGTH];

// Metadata structure for managing memory allocation
typedef struct Metadata {
    size_t size;       // Size of the chunk
    short is_free;     // Flag to indicate if the chunk is free
    struct Metadata* next; // Pointer to the next chunk of memory
} Metadata;

// Helper function to find the start of the memory pool
char* getMemoryStart() {
    return (char*)memory;
}

// Helper function to align size to the nearest multiple of 8
size_t alignSize(size_t size) {
    return (size + 7) & ~7; // Align size to 8 bytes
}

// Function to initialize memory if it's the first call to mymalloc
void initializeMemory() {
    Metadata* metadata = (Metadata*)getMemoryStart();
    metadata->size = MEMLENGTH * sizeof(double) - sizeof(Metadata); // Adjust for the size of the metadata
    metadata->is_free = 1;
    metadata->next = NULL; // Initially, there's no next chunk
}

// Custom malloc function
void* mymalloc(size_t size, char* file, int line) {
    if (size == 0) {
        printf("Attempt to allocate 0 bytes at %s:%d\n", file, line);
        return NULL;
    }

    // Ensure the size is aligned to 8 bytes
    size = alignSize(size);

    Metadata* metadata = (Metadata*)getMemoryStart();
    if (metadata->size == 0) { // If memory is uninitialized
        initializeMemory();
    }

    Metadata* current = metadata;
    while (current != NULL) {
        // Check if current block is free and large enough
        if (current->is_free && current->size >= size) {
            // Check if there's enough space to split the block
            if (current->size >= size + sizeof(Metadata) + 8) { // Ensure space for a new block and alignment
                Metadata* new_block = (Metadata*)((char*)current + sizeof(Metadata) + size);
                new_block->size = current->size - size - sizeof(Metadata);
                new_block->is_free = 1;
                new_block->next = current->next;
                
                current->size = size;
                current->next = new_block;
            }
            current->is_free = 0;
            return (void*)((char*)current + sizeof(Metadata));
        }
        current = current->next;
    }

    printf("Failed to allocate %zu bytes at %s:%d\n", size, file, line);
    return NULL;
}

// Custom free function
void myfree(void* ptr, char* file, int line) {
    if (!ptr) {
        printf("Attempt to free a NULL pointer at %s:%d\n", file, line);
        return;
    }

    char* heap_start = getMemoryStart();
    char* heap_end = heap_start + MEMLENGTH * sizeof(double);

    if ((char*)ptr < heap_start || (char*)ptr >= heap_end) {
        printf("Attempt to free pointer outside memory range at %s:%d\n", file, line);
        return;
    }

    // Get the metadata for the block being freed
    Metadata* block = (Metadata*)((char*)ptr - sizeof(Metadata));
    
    if (block->is_free) {
        printf("Attempt to free an already free pointer at %s:%d\n", file, line);
        return;
    }

    block->is_free = 1; // Mark the block as free

    // Coalesce adjacent free blocks
    Metadata* current = (Metadata*)getMemoryStart();
    while (current) {
        if (current->is_free && current->next && current->next->is_free) {
            current->size += current->next->size + sizeof(Metadata);
            current->next = current->next->next; // Remove the next block from the list
        } else {
            current = current->next;
        }
    }
}
