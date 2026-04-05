#ifndef AOCLIBS_PP_H_
#define AOCLIBS_PP_H_

#include "base.h"
#include "da.h"

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

#define POOL_NONE (1 << 0) // Pointers can be nullable, and freed by the user
#define POOL_NONNULL (1 << 1) // Pointers should only be cleaned up by this API

typedef struct {
    int flags;
    void *ptr;
} PFlags;

typedef struct {
    size_t cap;
    size_t len;
    PFlags *data;
} PPool;

void *palloc(PPool *pool, size_t size);

void *pnalloc(PPool *pool, size_t size);

void *pcalloc(PPool *pool, size_t nmeb, size_t size);

void *pncalloc(PPool *pool, size_t nmeb, size_t size);

// Frees all memory. Doesn't free the pool itself
// The user can always reuse the same pool
void pfree(PPool *pool);

// Discard the pool. User must free memory manually
void pdiscard(PPool *pool);

#ifdef AOCLIBS_IMPLEMENTATION
#include "pp.c"
#endif

#endif
