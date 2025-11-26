#ifndef AOCLIBS_HEAP_CALLOC_H
#define AOCLIBS_HEAP_CALLOC_H

#include <a_heap_types.h>
#include <a_error_types.h>

/*
 * High abstraction to calloc.
 * Heap allocate initiliazing all values to 0.
 * Return values:
 * 0 - The HeapPtr does not exist, or calloc failed.
 * 1 - A new pointer and size are set to HeapPtr.
 */
Error heap_clean_alloc(HeapPtr *h, usize count, usize size);

#endif
