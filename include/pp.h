#ifndef AOCLIBS_PP_H_
#define AOCLIBS_PP_H_

#include "base.h"

// Pointer Pool
//
// Arenas are handy for freeing all the memory at once and simplifying memory management.
// However, by using it we cannot reallocate freely anymore, and a huge work is necessary to keep
// the arena regions tidy.
//
// A pointer pool is a dynamic array that contains all pointers that has been allocated, so they
// can be freed one by one in a single function call.
//
// This is particularly useful, because there is no need for tracking the allocated memory anymore.
// And the memory is freed at the request moment of the program.
//
// Memory allocated using pfill is asserted to never be NULL.

typedef struct {
    size_t cap;
    size_t len;
    void **data;
} PPool;

AOCDEF void *pfill(PPool *pool, const size_t size);

AOCDEF void *pfillc(PPool *pool, const size_t nmeb, const size_t size);

AOCDEF void *prefresh(PPool *pool, void *ptr, const size_t size);

// Frees all memory. Doesn't free the pool itself
// The user can always reuse the same pool
AOCDEF void pdrain(PPool *pool);

#ifdef AOCLIBS_IMPLEMENTATION
#include "pp.c"
#endif

#endif
