#include <stdio.h>
#include <stdlib.h>

#include "alloc.h"

void *alloc(size_t size) {
    void *ptr = malloc(size);
    assertAlloc(ptr);

    return ptr;
}

void assertAlloc(void *ptr) {
    if (!ptr) {
        fprintf(stderr, "Fatal error: out of memory\n");
        exit(EXIT_FAILURE);
    }
}

void freeAlloc(void **ptr) {
    if (!ptr || !*ptr) return;
    
    free(*ptr);
    *ptr = NULL;
}