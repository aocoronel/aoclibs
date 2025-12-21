#ifndef AOCLIBS_ARENA_H
#define AOCLIBS_ARENA_H

#include <aoclibs/common.h>
#include <stdalign.h>

#define ARENA_DEFAULT_ALIGNMENT (alignof(void *))

typedef struct Arena {
        u8 *buffer;
        usize cap;
        usize offset;
} Arena;

Arena arena_create(usize cap);
void arena_reset(Arena *_Nonnull a);
void arena_destroy(Arena *_Nonnull a);
void *arena_alloc_aligned(Arena *_Nonnull a, usize size, usize align);
void *arena_alloc(Arena *_Nonnull a, usize size);
char *arena_alloc_chars(Arena *_Nonnull a, usize count);

#endif
