#ifndef AOCLIBS_HEAP_FREE_H
#define AOCLIBS_HEAP_FREE_H

#include <a_heap_types.h>

/*
 * High abstraction to free.
 * Frees given HeapPtr.
 * Sets the pointer to NULL and set the memory to 0.
 */
void heap_free(HeapPtr *h);

#endif
