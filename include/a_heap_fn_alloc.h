#ifndef AOCLIBS_HEAP_ALLOC_H
#define AOCLIBS_HEAP_ALLOC_H

#include <a_heap_types.h>
#include <a_error_types.h>

/*
 * High abstraction to malloc.
 *
 * Sets pointer to h.ptr, and it's capacity to h.mem
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null h assert.
 * - DEBUG_HEAP: Enable a_debug_heap.h
 *
 * Error.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
Error heap_malloc(HeapPtr *h, usize size);

#endif
