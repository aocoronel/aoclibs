#pragma once

#include "pp.h"

void *palloc(PPool *pool, size_t size) {
    void *tmp = malloc(size);
    if (!tmp) return NULL;

    PFlags f = { .flags = POOL_NONE, .ptr = tmp };
    da_insert(pool, f);
    return tmp;
}

void *pnalloc(PPool *pool, size_t size) {
    void *tmp = malloc(size);
    if (!tmp) return NULL;

    PFlags f = { .flags = POOL_NONNULL, .ptr = tmp };
    da_insert(pool, f);
    return tmp;
}

void *pcalloc(PPool *pool, size_t nmeb, size_t size) {
    void *tmp = calloc(nmeb, size);
    if (!tmp) return NULL;

    PFlags f = { .flags = POOL_NONE, .ptr = tmp };
    da_insert(pool, f);
    return tmp;
}

void *pncalloc(PPool *pool, size_t nmeb, size_t size) {
    void *tmp = calloc(nmeb, size);
    if (!tmp) return NULL;

    PFlags f = { .flags = POOL_NONNULL, .ptr = tmp };
    da_insert(pool, f);
    return tmp;
}

// Frees all memory. Doesn't free the pool itself
// The user can always reuse the same pool
void pfree(PPool *pool) {
    ASSERT(pool != NULL, "double free attempt");
    foreach (pool, i) {
        PFlags f = pool->data[i];
        if (f.flags & POOL_NONNULL) {
            ASSERT_NONNULL(f.ptr);
            free(f.ptr);
            f.ptr = NULL;
        } else {
            if (f.ptr) free(f.ptr);
            f.ptr = NULL;
        }
    }
}

// Discard the pool. User must free memory manually
void pdiscard(PPool *pool) {
    ASSERT(pool != NULL, "double free attempt");
    free(pool->data);
    pool->data = NULL;
    pool->len = 0;
    pool->cap = 0;
}
