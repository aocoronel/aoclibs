#ifndef AOCLIBS_HEAP_REALLOC_H
#define AOCLIBS_HEAP_REALLOC_H

#include <a_heap_types.h>
#include <a_error_types.h>

/*
 * High abstraction to realloc.
 *
 * Heap reallocate given HeapPtr to desired new size.
 * If realloc fails, the HeapPtr is untouched and still requires to be freed
 * with heap_free.
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
Error heap_realloc(HeapPtr *h, usize new_size);

#endif
