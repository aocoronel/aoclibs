#pragma once

#include "base.h"
#include "pp.h"
#include "da.h"

void *pfill(Pointer_Pool *pool, const size_t size) {
	$assert_nonnull(pool);

	void *tmp = malloc(size);
	$catch(!tmp) return NULL;

	da_insert(pool, tmp);
	return tmp;
}

void *pfillc(Pointer_Pool *pool, const size_t size) {
	$assert_nonnull(pool);

	void *tmp = malloc(size);
	$catch(!tmp) return NULL;

	memset(tmp, '\0', size);
	da_insert(pool, tmp);
	return tmp;
}

void *prefresh(Pointer_Pool *pool, size_t idx, const size_t size) {
	$assert_nonnull(pool);

	void *tmp = realloc(pool->data[idx], size);
	$catch(!tmp) return NULL;

	pool->data[idx] = tmp;

	return tmp;
}

void pdrain(Pointer_Pool *pool) {
	$assert(pool != NULL, "double free attempt");

	$range(0, pool->len, i) {
		$assert_nonnull(pool->data + i);
		free(pool->data[i]);
		pool->data[i] = NULL;
	}
	da_reset(pool);
}
