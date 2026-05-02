#pragma once

#include "base.h"
#include "pp.h"
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

void *prefresh(PPool *pool, size_t idx, const size_t size) {
    ASSERT_NONNULL(pool);

    void *tmp = realloc(pool->data[idx], size);
    if (!tmp) return NULL;

    pool->data[idx] = tmp;

    return tmp;
}

void pdrain(PPool *pool) {
    ASSERT(pool != NULL, "double free attempt");

    range(0, pool->len, i) {
        ASSERT_NONNULL(pool->data + i);
        free(pool->data[i]);
        pool->data[i] = NULL;
    }
    da_reset(pool);
}
