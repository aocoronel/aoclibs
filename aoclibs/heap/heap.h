#ifndef AOCLIBS_HEAP_H
#define AOCLIBS_HEAP_H

#include <aoclibs/error.h>
#include <aoclibs/int.h>

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
 *
 * - DEBUG_HEAP: Enable memory debugger
 */
void heap_free(Heap *h);

/*
 * High abstraction to malloc.
 *
 * Sets pointer to h.ptr, and it's capacity to h.mem
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null h assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.code:
 *
 * - Success: ok()
 * - Fail: errno
 */
Err heap_malloc(Heap *h, usize size);

/*
 * High abstraction to calloc.
 *
 * Heap allocate initiliazing all values to 0.
 *
 * Sets pointer to h.ptr, and it's capacity to h.mem
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null h assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.code:
 *
 * - Success: ok()
 * - Fail: errno
 */
Err heap_calloc(Heap *h, usize count, usize size);

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
 *
 * - NDEBUG: Disable debug prints. Disable non null h assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.code:
 *
 * - Success: ok()
 * - Fail: errno
 */
Err heap_realloc(Heap *h, usize new_size);

/* Errors */

#define HEAP_ERROR_LIST                 \
        X(AllocFailed, "allocation failed")

typedef enum {
#define X(enum_name, msg) Heap##enum_name,
        HEAP_ERROR_LIST
#undef X
} _HeapErr;

const char *const _HeapErrMsg[] = {
#define X(enum_name, msg) msg,
        HEAP_ERROR_LIST
#undef X
};

#define X(enum_name, m)                                 \
        const Err eHeap##enum_name = {                \
                .code = Heap##enum_name,              \
                .msg = _HeapErrMsg[Heap##enum_name] \
        };
HEAP_ERROR_LIST
#undef X

#endif
