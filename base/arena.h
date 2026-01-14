#ifndef AOCLIBS_ARENA_H_
#define AOCLIBS_ARENA_H_

#include "base.h"
#include <limits.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <unistd.h>
#include <sys/mman.h>
#endif

// Alignment of a pointer
#define AOC_ARENA_DEFAULT_ALIGNMENT (alignof(void *))

typedef struct Arena {
        int8_t *buffer;
        size_t cap;
        size_t offset;
} Arena;

#ifndef AOCLIBS_ARENA_ALLOCATOR
#define AOCLIBS_ARENA_ALLOCATOR malloc
#endif
#ifndef AOCLIBS_ARENA_REALLOCATOR
#define AOCLIBS_ARENA_REALLOCATOR realloc
#endif
#ifndef AOCLIBS_ARENA_FREE
#define AOCLIBS_ARENA_FREE free
#endif

#ifdef AOCLIBS_STRIP_PREFIX
#define arena_construct aoc_arena_construct
#define arena_alloc aoc_arena_alloc
#define arena_alloc_aligned aoc_arena_alloc_aligned
#define arena_alloc_chars aoc_arena_alloc_chars
#define arena_create aoc_arena_create
#define arena_cstrdup aoc_arena_cstrdup
#define arena_destroy aoc_arena_destroy
#define arena_realloc aoc_arena_realloc
#define arena_realloc_chars aoc_arena_realloc_chars
#define arena_reset aoc_arena_reset
#endif

// To generate a function that uses a global arena.
// This is particularly useful, if a library may accept an allocator compatible with the malloc
// function signature: void *malloc(size_t), so this one can be passed and the allocation
// happens in the Arena.
// The free function takes a pointer, as from void free(void *), but discards it
#define aoc_arena_construct(type, a, alignment)                                      \
        Arena a = { 0 };                                                             \
        AOCLIBS_PREFIX type arena_alloc_##a(size_t capacity) {                       \
                ASSERT(a.buffer != NULL, "the arena must be initialized with '" #a   \
                                         " = arena_create(size)', before usage");    \
                return (type)aoc_arena_alloc_aligned(&a, capacity, alignment);       \
        }                                                                            \
        AOCLIBS_PREFIX void arena_destroy_##a(void *buffer) {                        \
                (void)buffer;                                                        \
                aoc_arena_destroy(&a);                                               \
        }                                                                            \
        AOCLIBS_PREFIX void *arena_realloc_##a(void *buffer, size_t capacity) {      \
                return aoc_arena_realloc(&a, buffer, capacity, capacity, alignment); \
        }

// Usage:
// aoc_arena_construct(char *, arena_example, 1);
//
// int main(int argc, char *argv[]) {
//         arena_example = arena_create(1000); --> allocates in arena_example.buffer
//         char *msg = arena_alloc_arena_example(13); --> gives memory already allocated
//         strcpy(msg, "hello, world!");
//         printf("%s\n", msg);
//         msg = arena_realloc_arena_example(msg, 988); --> reallocates and gives memory already allocated
//         strcpy(msg, "hello, world once again!");
//         printf("%s\n", msg);
//         arena_destroy_arena_example(msg);
//         return 0;
// }

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

#ifdef __linux__
AOCLIBS_PREFIX Arena aoc_arena_create(size_t cap) {
        Arena a = { 0 };
        if (cap == 0) return a;

        a.buffer = mmap(NULL, cap, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (a.buffer == MAP_FAILED) {
                a.buffer = NULL;
                return a;
        }

        a.cap = cap;
        a.offset = 0;

        return a;
}
#else
AOCLIBS_PREFIX Arena aoc_arena_create(size_t cap) {
        Arena a = { 0 };
        if (cap == 0) return a;

        a.buffer = AOCLIBS_ARENA_ALLOCATOR(cap);
        if (!a.buffer) return a;

        a.cap = cap;
        a.offset = 0;

        return a;
}
#endif

AOCLIBS_PREFIX void aoc_arena_reset(Arena *ref a) {
        ASSERT_NONNULL(a != NULL);
        a->offset = 0;
}

#ifdef __linux__
AOCLIBS_PREFIX void aoc_arena_destroy(Arena *ref a) {
        ASSERT(a != NULL, "%s", "double free attempt");
        ASSERT(a->buffer != NULL, "%s", "double free attempt");
        munmap(a->buffer, a->cap);
        a->buffer = NULL;
        a->cap = 0;
        a->offset = 0;
}
#else
AOCLIBS_PREFIX void aoc_arena_destroy(Arena *ref a) {
        ASSERT(a != NULL, "%s", "double free attempt");
        ASSERT(a->buffer != NULL, "%s", "double free attempt");
        AOCLIBS_ARENA_FREE(a->buffer);
        a->buffer = NULL;
        a->cap = 0;
        a->offset = 0;
}
#endif

AOCLIBS_PREFIX void *null aoc_arena_alloc_aligned(Arena *ref a, size_t size, size_t align) {
        ASSERT_NONNULL(a != NULL);
        ASSERT((align & (align - 1)) == 0, "%s", "alignment is not a power of two");

        size_t curr = (size_t)(a->buffer + a->offset);
        size_t misalignment = curr & (align - 1);
        size_t padding = misalignment ? (align - misalignment) : 0;

        if (size > SIZE_MAX - a->offset - padding) return NULL;

        size_t needed = a->offset + padding + size;

        if (needed > a->cap) {
                size_t new_cap = a->cap ? a->cap * 2 : align;

                if (new_cap < needed) new_cap = needed;

                int8_t *new_buf = AOCLIBS_ARENA_REALLOCATOR(a->buffer, new_cap);
                if (!new_buf) return NULL;

                a->buffer = new_buf;
                a->cap = new_cap;
        }

        a->offset += padding;
        void *result = a->buffer + a->offset;
        a->offset += size;

        return result;
}

AOCLIBS_PREFIX void *aoc_arena_realloc(Arena *ref a, void *buffer, size_t buff_size,
                                       size_t new_size, size_t align) {
        ASSERT_NONNULL(a != NULL);
        void *tmp = aoc_arena_alloc_aligned(a, new_size, align);
        if (tmp == NULL) return NULL;
        memcpy(tmp, buffer, buff_size);
        return tmp;
}

AOCLIBS_PREFIX char *aoc_arena_realloc_chars(Arena *xref a, char *xref s, size_t s_cap,
                                             size_t new_size) {
        ASSERT_NONNULL(a != NULL);
        ASSERT_NONNULL(s != NULL);
        char *tmp = aoc_arena_alloc_chars(a, new_size);
        if (tmp == NULL) return NULL;
        if (s != NULL) {
                memcpy(tmp, s, s_cap);
        }
        return tmp;
}

AOCLIBS_PREFIX void *aoc_arena_alloc(Arena *ref a, size_t size) {
        ASSERT_NONNULL(a != NULL);
        return aoc_arena_alloc_aligned(a, size, AOC_ARENA_DEFAULT_ALIGNMENT);
}

AOCLIBS_PREFIX char *aoc_arena_alloc_chars(Arena *ref a, size_t count) {
        ASSERT_NONNULL(a != NULL);
        return (char *)aoc_arena_alloc_aligned(a, count, 1);
}

AOCLIBS_PREFIX char *aoc_arena_cstrdup(Arena *xref a, const char *xref s, size_t s_len) {
        ASSERT_NONNULL(a != NULL);
        ASSERT_NONNULL(s != NULL);
        char *tmp = (char *)aoc_arena_alloc_chars(a, s_len + 1);
        memcpy(tmp, s, s_len);
        tmp[s_len] = '\0';
        return tmp;
}

#endif // AOCLIBS_ARENA_H_
