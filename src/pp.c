#pragma once

#include "pp.h"
#include "base.h"
#include <stdio.h>
#include "da.h"

void *pfill(PPool *pool, const size_t size) {
    ASSERT_NONNULL(pool);

    void *tmp = malloc(size);
    if (!tmp) return NULL;

    da_insert(pool, tmp);
    return tmp;
}

void *pfillc(PPool *pool, const size_t nmeb, const size_t size) {
    ASSERT_NONNULL(pool);

    void *tmp = calloc(nmeb, size);
    if (!tmp) return NULL;

    da_insert(pool, tmp);
    return tmp;
}

void *prefresh(PPool *pool, void *ptr, const size_t size) {
    ASSERT_NONNULL(pool);

    void *tmp = realloc(ptr, size);
    if (!tmp) return NULL;

    bool found = false;
    for (size_t i = 0; i < pool->len && !found; i++) {
        void *p = pool->data[i];

        if (p == ptr) {
            pool->data[i] = tmp;
            found = true;
        }
    }

    if (!found) da_insert(pool, tmp);
    return tmp;
}

// Frees all memory. Doesn't free the pool itself
// The user can always reuse the same pool
void pdrain(PPool *pool) {
    ASSERT(pool != NULL, "double free attempt");

    range(0, pool->len, i) {
        ASSERT_NONNULL(pool->data + i);
        free(pool->data[i]);
        pool->data[i] = NULL;
    }
    da_reset(pool);
}
