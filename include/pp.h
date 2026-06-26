#ifndef AOC_PP_H_
#define AOC_PP_H_

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
//
// Memory allocated using pfill is asserted to never be NULL.

typedef struct {
	DYNAMIC_ARRAY(void *);
} PPool;

// Allocates 'size' bytes using malloc and stores pointer to pool
//
// NULL :: Out of memory
AOCDEF void *pfill(PPool *pool, const size_t size);

// Allocates zero initialized 'size' bytes using malloc and stores pointer to pool
//
// NULL :: Out of memory
AOCDEF void *pfillc(PPool *pool, const size_t size);

// Reallocates pointer at 'idx' using new 'size' in bytes
//
// NULL :: Out of memory
//
// Example:
//
// void *p = pfill(&pool, 100);
// size_t idx = pool.len - 1;
// p = prefresh(&pool, idx, 200);
AOCDEF void *prefresh(PPool *pool, size_t idx, const size_t size);

// Frees all memory. Doesn't free the pool itself.
//
// The user can always reuse the same pool.
//
// To free the pool, use da_free(pool)
AOCDEF void pdrain(PPool *pool);

#ifdef AOC_IMPLEMENTATION
#include "pp.c"
#endif

#endif
