#ifndef alloc_h
#define alloc_h

#include <stdlib.h>

// convenience macro for freeing allocated memory
#define FREE_ALLOC(ptr) freeAlloc((void**)&(ptr))

// allocates memory and exits the program if allocation fails
void *alloc(size_t size);

// checks if a pointer is NULL and exits the program if it is
// calling 'alloc' checks this anyway, so only use for other allocation functions, 'strdup', etc
void assertAlloc(void *ptr);

// frees allocated memory
void freeAlloc(void **ptr);

#endif