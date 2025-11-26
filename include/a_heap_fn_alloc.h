#ifndef AOCLIBS_HEAP_ALLOC_H
#define AOCLIBS_HEAP_ALLOC_H

#include <a_heap_types.h>
#include <a_error_types.h>

/*
 * High abstraction to malloc.
 * Heap allocate without initiliazing all values to 0.
 * Return values:
 * 0 - The HeapPtr does not exist, or malloc failed.
 * 1 - A new pointer and size are set to HeapPtr.
 */
Error heap_alloc(HeapPtr *h, usize size);

#endif
