#include <stdio.h>
#include "cstr.h"
#include "base.h"
#include "arena.h"

static Arena default_arena = { 0 };
static Arena temporary_arena = { 0 };
static Arena *context_arena = &default_arena;

typedef struct {
        size_t len;
        size_t cap;
        int *data;
} DaInt;

typedef struct {
        size_t len;
        size_t cap;
        char *data;
} DaStr;

void *context_alloc(size_t size) {
        assert(context_arena);
        return arena_alloc(context_arena, size);
}

int main(void) {
        // Allocate stuff in default_arena
        context_alloc(64);
        context_alloc(128);
        context_alloc(256);
        context_alloc(512);

        // Allocate stuff in temporary_arena;
        context_arena = &temporary_arena;
        context_alloc(64);
        context_alloc(128);
        context_alloc(256);
        context_alloc(512);

        DaInt myda = { 0 };
        arena_da_insert(&default_arena, &myda, 64);
        arena_da_insert(&default_arena, &myda, 128);
        arena_da_insert(&default_arena, &myda, 256);
        arena_da_insert(&default_arena, &myda, 512);

        foreach (&myda, i) {
                printf("%d\n", (int)myda.data[i]);
        }

        DaStr myda_str = { 0 };
        arena_lrc_copy(&default_arena, &myda_str, "hello, world");

        printf("%*s\n", (int)myda_str.len, (char *)myda_str.data);

        // Deallocate everything at once
        arena_destroy(&default_arena);
        arena_destroy(&temporary_arena);
        return 0;
}
