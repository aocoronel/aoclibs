#ifndef AOCLIBS_VEC_H_
#define AOCLIBS_VEC_H_

#include <errno.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attributes.h"
#include "assert.h"

typedef int (*VecCompare)(const void *a, const void *b);

typedef struct {
        void *data;
        size_t len;
        size_t cap;
        size_t pad; // Size of each element in bytes
} Vec;

Vec aoc_vec_init(size_t pad);
void aoc_vec_free(Vec *ref v);

// if max allocation capacity by system is reached, returns -1
int aoc_vec_reserve(Vec *ref v, size_t cap) FN_WARN_UNUSED;

// if max allocation capacity by system is reached, returns -1
int aoc_vec_realloc(Vec *ref v) FN_WARN_UNUSED;

void *aoc_vec_at(Vec *ref v, size_t index) FN_WARN_UNUSED;

int aoc_vec_push(Vec *ref v, const void *value) FN_WARN_UNUSED;

// char not found -1
int aoc_vec_pop(Vec *ref v, void *ref output) FN_WARN_UNUSED;

// index doesn't exist -1
int aoc_vec_insert(Vec *ref v, size_t index, const void *null value) FN_WARN_UNUSED;

void aoc_vec_swap(Vec *ref v, size_t a, size_t b);

// value not found -1
size_t aoc_vec_partition(Vec *ref v, VecCompare cmp, size_t lo, size_t hi);

void aoc_vec_qsort(Vec *ref v, VecCompare cmp, size_t lo, size_t hi);

void aoc_vec_sort(Vec *ref v, VecCompare cmp);

// nothing to shrink -1
int aoc_vec_shrink_to_fit(Vec *ref v) FN_WARN_UNUSED;

void aoc_vec_clear(Vec *ref v);

// index doesn't exist -1
int aoc_vec_erase(Vec *v, size_t index) FN_WARN_UNUSED;

int aoc_vec_resize(Vec *ref v, size_t new_len, const void *null value) FN_WARN_UNUSED;

// padding mismatch -1
int aoc_vec_copy(Vec *xref dest, Vec *xref src) FN_WARN_UNUSED;

int aoc_vec_equal(Vec *a, Vec *b, VecCompare cmp);

void aoc_vec_for_each(Vec *v, void (*fn)(void *));

#endif // AOCLIBS_VEC_H_
