#ifndef AOCLIBS_HEAP_FREE_H
#define AOCLIBS_HEAP_FREE_H

#include <a_heap_types.h>

/*
 * High abstraction to free.
 *
 * Frees given HeapPtr.
 *
 * Sets the pointer to NULL and set the memory to 0.
 *
 * If h or h.ptr is NULL, returns earlier
 *
 * Defines:
 *
 * - DEBUG_HEAP: Enable a_debug_heap.h
 */
void heap_free(HeapPtr *h);

#endif
