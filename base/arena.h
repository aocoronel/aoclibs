#ifndef AOCLIBS_ARENA_H_
#define AOCLIBS_ARENA_H_

#define _GNU_SOURCE
#include "base.h"
#include <assert.h>
#include <limits.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef AOCLIBS_ARENA_LINUX_USES_MMAP
#ifdef __linux__
#include <unistd.h>
#include <sys/mman.h>
#endif
#endif

/*
 * Arena Implementation
*/

// Alignment of a pointer
#define AOC_ARENA_DEFAULT_ALIGNMENT (alignof(void *))

typedef struct Arena {
        int8_t *buffer;
        size_t cap;
        size_t offset;
} Arena;

typedef struct {
        Arena *arena; // You must set the arena before using
        size_t *offset;
        size_t len; // n of values allocated
        size_t cap; // max n of values
} DynamicArena;

#ifndef AOCLIBS_ARENA_ALLOCATOR
#define AOCLIBS_ARENA_ALLOCATOR malloc
#endif
#ifndef AOCLIBS_ARENA_REALLOC
#define AOCLIBS_ARENA_REALLOC realloc
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
 * Duplicates a value using an arena
*/
#define aoc_arena_dup(a, tmp, value, size, alignment)                    \
        do {                                                             \
                tmp = aoc_arena_alloc_aligned((a), (size), (alignment)); \
                memcpy(tmp, (value), (size));                            \
        } while (0)

/*
 * Creates an arena of size (cap).
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

/*
 * Guarantees DAR can store up to 'new_cap' offsets
 *
 * Defines:
 * - NDEBUG :: Disable assertions
*/
void _aoc_dar_reserve(aoc_realloc_t realloc, DynamicArena *ref dar, size_t new_cap);
/*
 * Frees DAR, reseting all values, but the arena
 *
 * Defines:
 * - NDEBUG :: Disable assertions
*/
void _aoc_dar_free(aoc_free_t free, DynamicArena *ref dar);

#ifdef AOCLIBS_IMPLEMENTATION
#if defined(__linux__) && defined(AOCLIBS_ARENA_LINUX_USES_MMAP)
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

#if defined(__linux__) && defined(AOCLIBS_ARENA_LINUX_USES_MMAP)
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
#if defined(__linux__) && defined(AOCLIBS_ARENA_LINUX_USES_MMAP)
                int old_cap = a->cap;
#endif
                while (needed > a->cap) {
                        a->cap *= 2;
                }

#if defined(__linux__) && defined(AOCLIBS_ARENA_LINUX_USES_MMAP)
                int8_t *new_buf = mremap(a->buffer, old_cap, a->cap, MREMAP_MAYMOVE);
#else
                int8_t *new_buf = AOCLIBS_ARENA_REALLOC(a->buffer, a->cap);
#endif
                if (!new_buf) return NULL;

                a->buffer = new_buf;
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
        char *tmp;
        aoc_arena_dup(a, tmp, s, s_len + 1, 1);
        tmp[s_len] = '\0';
        return tmp;
}
#endif

/*
 * Dynamic Arena Implementation
 *
 * Acts like a Dynamic Array, but uses the Arena for allocation
 *
 * The whole implementation is done using macros, which allows to be used with any type the user
 * needs.
 *
 * Below is the a very generic dynamic arena struct. The data type can be swapped to something else
 * and it's going to work just like normal.
*/

// This capacity is how many items you have to allocate, before resizing the the Dynamic Arena.
// This is not related to the sizeof the type or the allocated size in the memory.
#define AOCLIBS_DAR_INITIAL_CAPACITY 8

#define AOCLIBS_DAR_REALLOC realloc
#define AOCLIBS_DAR_FREE free

// The following macros use the allocators defined above.
//
// Macros starting with "_" accept an extra argument: realloc
// Except for some like the very following ones, which are internal

// Convenient assertions to prevent access out of bounds
#define _aoc_assert_dar_is_valid(dar) (assert((dar)->len > 0 && "out of bounds access"))
#define _aoc_assert_dar_index_is_valid(dar, i) (assert((dar)->len >= i && "out of bounds access"))
#define _aoc_assert_dar_safe(dar, i) \
        (_aoc_assert_dar_is_valid(dar), _aoc_assert_dar_index_is_valid(dar, i))
#define _aoc_assert_dar_pop(dar) assert((dar)->len - 1 != SIZE_MAX && "out of bounds access")

// Before any attempt to add new values to the DAR, always reserve the memory.
//
// DynamicArena my_dar = { .arena = &arena };
// dar_reserve(&my_dar, (&my_dar)->len + 1); // Needs to allocate one value
#define aoc_dar_reserve(dar, new_cap) _aoc_dar_reserve(AOCLIBS_DAR_REALLOC, dar, new_cap)
#ifdef AOCLIBS_IMPLEMENTATION
AOCLIBS_PREFIX void _aoc_dar_reserve(aoc_realloc_t realloc, DynamicArena *ref dar, size_t new_cap) {
        ASSERT_NONNULL(dar != NULL);
        if ((new_cap) > dar->cap) {
                if (dar->cap == 0) dar->cap = AOCLIBS_DAR_INITIAL_CAPACITY;
                while (new_cap > dar->cap)
                        dar->cap *= 2;
                dar->offset = realloc(dar->offset, dar->cap * sizeof(*dar->offset));
        }
}
#endif

#define aoc_dar_free(dar) _aoc_dar_free(AOCLIBS_DAR_FREE, dar)
#ifdef AOCLIBS_IMPLEMENTATION
AOCLIBS_PREFIX void _aoc_dar_free(aoc_free_t free, DynamicArena *ref dar) {
        ASSERT(dar != NULL, "double free attempt");
        free(dar->offset);
        dar->offset = NULL;
        dar->len = 0;
        dar->cap = 0;
}
#endif

// Returns last pointer
#define aoc_dar_last(dar) \
        ((dar)->arena->buffer + (dar)->offset[_aoc_assert_dar_is_valid(dar), (dar)->len - 1])

// Returns pointer from given index
#define aoc_dar_get(dar, index) \
        ((dar)->arena->buffer + (dar)->offset[_aoc_assert_dar_safe(dar, index), index])

// Returns last pointer, and removes it from the DAR
#define aoc_dar_pop(dar) \
        ((dar)->arena->buffer + (dar)->offset[_aoc_assert_dar_pop(dar), --(dar)->len])

// Empties the DAR
#define aoc_dar_clear(dar) (dar)->len = 0

// Sets item to given index in the DAR
#define aoc_dar_set(Type, dar, i, item)                                      \
        do {                                                                 \
                Type _tmp = (Type)((dar)->arena->buffer + (dar)->offset[i]); \
                *_tmp = item;                                                \
        } while (0)

// Append any type
#define aoc_dar_append(dar, item, item_size) \
        _aoc_dar_append(AOCLIBS_DAR_REALLOC, (dar), (item), (item_size))

#define _aoc_dar_append(realloc, dar, item, item_size)                                        \
        do {                                                                                  \
                _aoc_dar_reserve((realloc), (dar), (dar)->len + 1);                           \
                void *tmp = aoc_arena_alloc_aligned((dar)->arena, item_size,                  \
                                                    sizeof((dar)->offset[0]));                \
                memcpy(tmp, item, item_size);                                                 \
                (dar)->offset[(dar)->len++] = (size_t)((int8_t *)tmp - (dar)->arena->buffer); \
        } while (0)

// Append C string
#define aoc_dar_append_cstr(dar, cstr, cstr_len) \
        _aoc_dar_append_cstr(AOCLIBS_DAR_REALLOC, (dar), (cstr), (cstr_len))

#define _aoc_dar_append_cstr(realloc, dar, cstr, cstr_len)                                    \
        do {                                                                                  \
                _aoc_dar_reserve((realloc), (dar), (dar)->len + 1);                           \
                char *ptr = aoc_arena_cstrdup((dar)->arena, cstr, cstr_len);                  \
                (dar)->offset[(dar)->len++] = (size_t)((int8_t *)ptr - (dar)->arena->buffer); \
        } while (0)

// Append C string literal
#define aoc_dar_append_cstrl(dar, cstr) \
        _aoc_dar_append_cstr(AOCLIBS_DAR_REALLOC, dar, cstr, cstrl_len(cstr))
#define _aoc_dar_append_cstrl(realloc, dar, cstr) \
        _aoc_dar_append_cstr(realloc, dar, cstr, cstrl_len(cstr))

// Append all values from buffer
#define aoc_dar_append_many(dar, item, item_count, size, alignment) \
        _aoc_dar_append_many(AOCLIBS_DAR_REALLOC, (dar), (item), (item_count), (size), (alignment))

#define _aoc_dar_append_many(realloc, dar, item, item_count, size, alignment)                \
        do {                                                                                 \
                for (size_t _i = 0; _i < (item_count); _i++) {                               \
                        _aoc_dar_reserve((realloc), (dar), (dar)->len + 1);                  \
                        void *_tmp = aoc_arena_alloc_aligned((dar)->arena, size, alignment); \
                        if (!_tmp) break;                                                    \
                        (dar)->offset[(dar)->len++] =                                        \
                                (size_t)((int8_t *)_tmp - (dar)->arena->buffer);             \
                        memcpy(_tmp, (char *)(item) + _i * (size), size);                    \
                }                                                                            \
        } while (0)

// Append all strings from buffer
#define aoc_dar_append_many_cstr(dar, cstr, cstrs_len) \
        _aoc_dar_append_many_cstr(AOCLIBS_DAR_REALLOC, (dar), (cstr), (cstrs_len))

#define _aoc_dar_append_many_cstr(realloc, dar, cstrs, cstrs_len)                                  \
        do {                                                                                       \
                for (size_t _i = 0; _i < (cstrs_len); _i++) {                                      \
                        _aoc_dar_reserve((realloc), (dar), (dar)->len + 1);                        \
                        void *_tmp =                                                               \
                                aoc_arena_cstrdup((dar)->arena, (cstrs)[_i], strlen((cstrs)[_i])); \
                        (dar)->offset[(dar)->len++] =                                              \
                                (size_t)((int8_t *)_tmp - (dar)->arena->buffer);                   \
                }                                                                                  \
        } while (0)

// Swaps offset of i1 to i2
//
// If you have already got a pointer using dar_get, you have to update it
#define aoc_dar_swap(dar, i1, i2)                                       \
        do {                                                            \
                aoc_swap(size_t, (dar)->offset[i1], (dar)->offset[i2]); \
        } while (0)

#ifdef AOCLIBS_STRIP_PREFIX
#define _dar_append _aoc_dar_append
#define _dar_append_cstr _aoc_dar_append_cstr
#define _dar_append_many _aoc_dar_append_many
#define _dar_append_many_cstr _aoc_dar_append_many_cstr
#define _dar_free _aoc_dar_free
#define _dar_reserve _aoc_dar_reserve

#define dar_append aoc_dar_append
#define dar_append_cstr aoc_dar_append_cstr
#define dar_append_many aoc_dar_append_many
#define dar_append_many_cstr aoc_dar_append_many_cstr
#define dar_append_cstrl aoc_dar_append_cstrl
#define dar_free aoc_dar_free
#define dar_reserve aoc_dar_reserve

#define dar_get aoc_dar_get
#define dar_set aoc_dar_set
#define dar_clear aoc_dar_clear
#define dar_foreach aoc_dar_foreach
#define dar_last aoc_dar_last
#define dar_pop aoc_dar_pop
#define dar_swap aoc_dar_swap
#endif

#endif // AOCLIBS_ARENA_H_
