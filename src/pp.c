#pragma once

#include "pp.h"
#include <stdio.h>
#include "da.h"

void *pfill(PPool *pool, size_t size) {
    void *tmp = malloc(size);
    if (!tmp) return NULL;

    da_insert(pool, tmp);
    return tmp;
}

void *pfillc(PPool *pool, size_t nmeb, size_t size) {
    void *tmp = calloc(nmeb, size);
    if (!tmp) return NULL;

    da_insert(pool, tmp);
    return tmp;
}

// Frees all memory. Doesn't free the pool itself
// The user can always reuse the same pool
void pdrain(PPool *pool) {
    ASSERT(pool != NULL, "double free attempt");
    foreach (pool, i) {
        ASSERT_NONNULL(pool->data + i);
        free(pool->data[i]);
        pool->data[i] = NULL;
    }
    da_reset(pool);
}
