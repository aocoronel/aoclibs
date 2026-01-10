#ifndef AOCLIBS_ARENA_H_
#define AOCLIBS_ARENA_H_

#include "attributes.h"
#include <stddef.h>
#include <stdint.h>

// Alignment of a pointer
#define AOC_ARENA_DEFAULT_ALIGNMENT (alignof(void *))

typedef struct Arena {
        int8_t *buffer;
        size_t cap;
        size_t offset;
} Arena;

/*
 * Creates an arena of size (cap).
 *
 * Defines:
 * - HEAP_TRACE :: Enables heap tracing
 *
 * Errors:
 * - If allocation fails, the arena is returned zero initialized
*/
AOCLIBS_PREFIX Arena aoc_arena_create(size_t cap);

/*
 * Resets the offset to zero. This function does not free memory.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
*/
AOCLIBS_PREFIX void aoc_arena_reset(Arena *ref a);

/*
 * Frees the arena, and set all values to zero.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
*/
AOCLIBS_PREFIX void aoc_arena_destroy(Arena *ref a);

/*
 * Returns a pointer from the arena with the requested size and alignment.
 *
 * This function may reallocate, if necessary.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - If reallocation fails or requested size is bigger than SIZE_MAX - offset - padding, then returns
 *   NULL
*/
AOCLIBS_PREFIX void *aoc_arena_alloc_aligned(Arena *ref a, size_t size, size_t align);

/*
 * Returns a pointer from the arena with the requested size. This function uses the default alignment.
 *
 * This function may reallocate, if necessary.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - If reallocation fails or requested size is bigger than SIZE_MAX - offset - padding, then returns
 *   NULL
*/
AOCLIBS_PREFIX void *aoc_arena_alloc(Arena *ref a, size_t size);

/*
 * Returns a pointer from the arena with the requested size. This function uses the alignment of a
 * char.
 *
 * This function may reallocate, if necessary.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - If reallocation fails or requested size is bigger than SIZE_MAX - offset - padding, then returns
 *   NULL
*/
AOCLIBS_PREFIX char *aoc_arena_alloc_chars(Arena *ref a, size_t count);

#ifdef AOCLIBS_STRIP_PREFIX
#define arena_create(cap) aoc_aoc_arena_create((cap))
#define arena_reset(arena) aoc_areaoc_arena_reset((arena))
#define arena_destroy(arena) aoc_arena_desaoc_arena_destroy((arena))
#define arena_alloc_aligned(arena, size, align) \
        aoc_arena_allaoc_arena_alloc_aligned((arena), (size), (align))
#define arena_alloc(arena, size) aoc_arena_allocaoc_arena_alloc((arena), (size))
#define arena_alloc_chars(arena, count) aoc_arenaaoc_arena_alloc_chars((arena), (count))
#endif

#endif // AOCLIBS_ARENA_H_
