#ifndef AOCLIBS_HEAP_H
#define AOCLIBS_HEAP_H

#include <aoclibs/common.h>

/*
 * Defines a pointer used for heap allocation, which is followed by it's size.
 */
typedef struct Heap {
    void *ptr;
    usize cap; // current allocated size
} Heap;

/*
 * High abstraction to free.
 *
 * Frees given Heap.
 *
 * Sets the pointer to NULL and set the memory to 0.
 *
 * If h or h.ptr is NULL, returns earlier
 *
 * Defines:
 * - DEBUG_HEAP :: Enable memory debugger
 */
void heap_free(Heap *_Nonnull h);

/*
 * High abstraction to malloc.
 *
 * Sets pointer to h.ptr, and it's capacity to h.mem
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << malloc
 */
Err heap_malloc(Heap *_Nonnull h, usize size);

/*
 * High abstraction to calloc.
 *
 * Heap allocate initiliazing all values to 0.
 *
 * Sets pointer to h.ptr, and it's capacity to h.mem
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << calloc
 */
Err heap_calloc(Heap *_Nonnull h, usize count, usize size);

/*
 * High abstraction to realloc.
 *
 * Heap reallocate given Heap to desired new size.
 * If realloc fails, the Heap is untouched and still requires to be freed
 * with heap_free.
 *
 * Sets pointer to h.ptr, and it's capacity to h.mem
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << realloc
 */
Err heap_realloc(Heap *_Nonnull h, usize new_size);

#endif
