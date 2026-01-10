#include "attributes.h"
#include "assert.h"
#include "arena.h"

#include <limits.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

AOCLIBS_PREFIX Arena aoc_arena_create(size_t cap) {
        Arena a = { 0 };
        if (cap == 0) return a;

        a.buffer = malloc(cap);
        if (!a.buffer) return a;

        a.cap = cap;
        a.offset = 0;

        return a;
}

AOCLIBS_PREFIX void aoc_arena_reset(Arena *ref a) {
        ASSERT_NONNULL(a != NULL);
        a->offset = 0;
}

AOCLIBS_PREFIX void aoc_arena_destroy(Arena *ref a) {
        ASSERT(a != NULL, "%s", "double free attempt");
        ASSERT(a->buffer != NULL, "%s", "double free attempt");
        free(a->buffer);
        a->buffer = NULL;
        a->cap = 0;
        a->offset = 0;
}

AOCLIBS_PREFIX void *aoc_arena_alloc_aligned(Arena *ref a, size_t size, size_t align) {
        ASSERT(a != NULL, "%s", "double free attempt");
        ASSERT((align & (align - 1)) == 0, "%s", "alignment is not a power of two");

        size_t curr = (size_t)(a->buffer + a->offset);
        size_t misalignment = curr & (align - 1);
        size_t padding = misalignment ? (align - misalignment) : 0;

        if (size > SIZE_MAX - a->offset - padding) return NULL;

        size_t needed = a->offset + padding + size;

        if (needed > a->cap) {
                size_t new_cap = a->cap ? a->cap * 2 : align;

                if (new_cap < needed) new_cap = needed;

                int8_t *new_buf = realloc(a->buffer, new_cap);
                if (!new_buf) return NULL;

                a->buffer = new_buf;
                a->cap = new_cap;
        }

        a->offset += padding;
        void *result = a->buffer + a->offset;
        a->offset += size;

        return result;
}

AOCLIBS_PREFIX void *aoc_arena_alloc(Arena *ref a, size_t size) {
        return aoc_arena_alloc_aligned(a, size, AOC_ARENA_DEFAULT_ALIGNMENT);
}

AOCLIBS_PREFIX char *aoc_arena_alloc_chars(Arena *ref a, size_t count) {
        return (char *)aoc_arena_alloc_aligned(a, count, 1);
}
