#include <aoclibs/common.h>
#include <aoclibs/heap/arena.h>
#include <errno.h>
#include <limits.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Arena arena_create(usize cap) {
        Arena a = { 0 };
        if (cap == 0) a;

        a.buffer = malloc(cap);
        if (!a.buffer) return a;

        a.cap = cap;
        a.offset = 0;

        return a;
}

void arena_reset(Arena *_Nonnull a) {
        ASSERT(a != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        a->offset = 0;
}

void arena_destroy(Arena *_Nonnull a) {
        ASSERT(a != NULL, "%s", "double free attempt");
        free(a->buffer);
        a->buffer = NULL;
        a->cap = 0;
        a->offset = 0;
}

void *arena_alloc_aligned(Arena *_Nonnull a, usize size, usize align) {
        ASSERT(a != NULL, "%s", "double free attempt");
        ASSERT((align & (align - 1)) == 0, "%s", "alignment is not a power of two");

        usize curr = (usize)(a->buffer + a->offset);
        usize misalignment = curr & (align - 1);
        usize padding = misalignment ? (align - misalignment) : 0;

        if (size > SIZE_MAX - a->offset - padding) return NULL;

        usize needed = a->offset + padding + size;

        if (needed > a->cap) {
                usize new_cap = a->cap ? a->cap * 2 : align;

                if (new_cap < needed) new_cap = needed;

                u8 *new_buf = realloc(a->buffer, new_cap);
                if (!new_buf) return NULL;

                a->buffer = new_buf;
                a->cap = new_cap;
        }

        a->offset += padding;
        void *result = a->buffer + a->offset;
        a->offset += size;

        return result;
}

void *arena_alloc(Arena *_Nonnull a, usize size) {
        return arena_alloc_aligned(a, size, ARENA_DEFAULT_ALIGNMENT);
}

char *arena_alloc_chars(Arena *_Nonnull a, usize count) {
        return (char *)arena_alloc_aligned(a, count, 1);
}
