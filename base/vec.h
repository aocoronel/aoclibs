#ifndef AOCLIBS_VEC_H_
#define AOCLIBS_VEC_H_

#include "base.h"
#include <errno.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*VecCompare)(const void *a, const void *b);

typedef struct {
        void *data;
        size_t len;
        size_t cap;
        size_t pad; // Size of each element in bytes
} Vec;

#define AOCLIBS_VECTOR_GROWTH_FACTOR 2

#ifdef AOCLIBS_STRIP_PREFIX
#define vec_init aoc_vec_init
#define vec_free aoc_vec_free
#define vec_reserve aoc_vec_reserve
#define vec_realloc aoc_vec_realloc
#define vec_at aoc_vec_at
#define vec_push aoc_vec_push
#define vec_pop aoc_vec_pop
#define vec_insert aoc_vec_insert
#define vec_swap aoc_vec_swap
#define vec_partition aoc_vec_partition
#define vec_qsort aoc_vec_qsort
#define vec_sort aoc_vec_sort
#define vec_shrink_to_fit aoc_vec_shrink_to_fit
#define vec_clear aoc_vec_clear
#define vec_erase aoc_vec_erase
#define vec_copy aoc_vec_copy
#define vec_equal aoc_vec_equal
#define vec_for_each aoc_vec_for_each
#endif

/*
 * Initialize a new vector with corresponding padding.
 *
 * The padding refers to the size of the type you want to put in the vector.
 *
 * The vector allocates a single time and grows overtime, and can only store data with the same
 * padding.
*/
AOCLIBS_PREFIX Vec aoc_vec_init(size_t pad);

/*
 * Frees a vector and sets all fields to zero.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
*/
AOCLIBS_PREFIX void aoc_vec_free(Vec *ref v);

/*
 * Realloc the size of the vector with custom new capacity.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: max allocation capacity by system is reached
*/
AOCLIBS_PREFIX int aoc_vec_reserve(Vec *ref v, size_t new_len);

/*
 * Realloc the size of the vector with default growth factor.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: max allocation capacity by system is reached
*/
AOCLIBS_PREFIX int aoc_vec_realloc(Vec *ref v) FN_WARN_UNUSED;

/*
 * Returns values at requested index
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - NULL :: index is bigger than vector length
*/
AOCLIBS_PREFIX void *aoc_vec_at(Vec *ref v, size_t index) FN_WARN_UNUSED;

/*
 * Pushes values to vector
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: max allocation capacity by system is reached
*/
AOCLIBS_PREFIX int aoc_vec_push(Vec *ref v, const void *value) FN_WARN_UNUSED;

/*
 * Pops value from vector to output.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: vector is empty
*/
AOCLIBS_PREFIX int aoc_vec_pop(Vec *ref v, void *null output) FN_WARN_UNUSED;

/*
 * Inserts values to vector at specific index
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: index is bigger than vector length
*/
AOCLIBS_PREFIX int aoc_vec_insert(Vec *ref v, size_t index, void *value) FN_WARN_UNUSED;

/*
 * Swaps the position two values from vector
 *
 * Defines:
 * - NDEBUG :: Disable assertions
*/
AOCLIBS_PREFIX void aoc_vec_swap(Vec *ref v, size_t a, size_t b);

/*
 * Partitions the vector
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: index is bigger than vector length
*/
AOCLIBS_PREFIX size_t aoc_vec_partition(Vec *ref v, VecCompare cmp, size_t lo, size_t hi);

/*
 * Sort part of the vector
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - NULL :: low end is larger than the higher end (lo >= hi)
*/
AOCLIBS_PREFIX void aoc_vec_qsort(Vec *ref v, VecCompare cmp, size_t lo, size_t hi);

/*
 * Sort the vector
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - NULL :: low end is larger than the higher end (lo >= hi)
*/
AOCLIBS_PREFIX void aoc_vec_sort(Vec *ref v, VecCompare cmp);

/*
 * Shrink the size of the vector, freeing unused memory.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: nothing to shrink (v->len >= v->cap)
*/
AOCLIBS_PREFIX int aoc_vec_shrink_to_fit(Vec *ref v) FN_WARN_UNUSED;

/*
 * Clears the vector without freeing memory.
*/
AOCLIBS_PREFIX void aoc_vec_clear(Vec *ref v);

/*
 * Pops a value from specified index
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - NULL :: index is bigger than vector length
*/
AOCLIBS_PREFIX int aoc_vec_erase(Vec *v, size_t index) FN_WARN_UNUSED;

/*
 * Copies all contents from src to dest
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: max allocation capacity by system is reached
 * - -2 :: padding mismatch
*/
AOCLIBS_PREFIX int aoc_vec_copy(Vec *xref dest, Vec *xref src) FN_WARN_UNUSED;

/*
 * Checks if vectors are equal in contents, padding and length
 *
 * Defines:
 * - NDEBUG :: Disable assertions
*/
AOCLIBS_PREFIX bool aoc_vec_equal(Vec *a, Vec *b, VecCompare cmp);

/*
 * Run function for each element from the vector
*/
AOCLIBS_PREFIX void aoc_vec_for_each(Vec *v, void (*fn)(void *));

AOCLIBS_PREFIX Vec aoc_vec_init(size_t pad) {
        return (Vec){ .data = NULL, .len = 0, .cap = 0, .pad = pad };
}

AOCLIBS_PREFIX void aoc_vec_free(Vec *ref v) {
        ASSERT(v != NULL, "%s", "double free attempt");
        free(v->data);
        *v = (Vec){ 0 };
}

AOCLIBS_PREFIX int aoc_vec_reserve(Vec *ref v, size_t new_len) {
        ASSERT_NONNULL(v != NULL);

        if (new_len > SIZE_MAX / v->pad) return -1;

        void *tmp_ptr = realloc(v->data, new_len * v->pad);
        if (!tmp_ptr) return errno;

        void *new_data = tmp_ptr;

        v->data = new_data;
        v->cap = new_len;
        return 0;
}

AOCLIBS_PREFIX int aoc_vec_realloc(Vec *ref v) {
        ASSERT_NONNULL(v != NULL);

        size_t new_cap = v->cap ? v->cap * AOCLIBS_VECTOR_GROWTH_FACTOR : 8;

        if (new_cap > SIZE_MAX / v->pad) return -1;

        void *new_data = realloc(v->data, new_cap * v->pad);
        if (!new_data) return errno;

        v->data = new_data;
        v->cap = new_cap;
        return 0;
}

AOCLIBS_PREFIX void *aoc_vec_at(Vec *ref v, size_t index) {
        ASSERT_NONNULL(v != NULL);
        if (index > v->len) return NULL;

        return (char *)v->data + index * v->pad;
}

AOCLIBS_PREFIX int aoc_vec_push(Vec *ref v, const void *value) {
        ASSERT_NONNULL(v != NULL);
        int err = aoc_vec_reserve(v, v->len + 1);
        if (err != 0) return err;
        char *dest = (char *)v->data + v->len * v->pad;
        memcpy(dest, value, v->pad);
        v->len++;
        return 0;
}

AOCLIBS_PREFIX int aoc_vec_pop(Vec *ref v, void *null output) {
        ASSERT_NONNULL(v != NULL);
        if (v->len == 0) return -1;
        v->len--;
        if (output) {
                memcpy(output, (char *)v->data + v->len * v->pad, v->pad);
        }
        return 0;
}

AOCLIBS_PREFIX int aoc_vec_insert(Vec *ref v, size_t index, void *value) {
        ASSERT_NONNULL(v != NULL);
        if (index > v->len) return -1;
        int err = aoc_vec_reserve(v, v->len + 1);
        if (err != 0) return err;

        void *dst = (char *)v->data + (index + 1) * v->pad;
        void *src = (char *)v->data + index * v->pad;
        size_t move = (v->len - index) * v->pad;

        memmove(dst, src, move);
        memcpy(src, value, v->pad);
        v->len++;
        return 0;
}

AOCLIBS_PREFIX void aoc_vec_swap(Vec *ref v, size_t a, size_t b) {
        ASSERT_NONNULL(v != NULL);
        if (a == b) return;

        char *pa = (char *)v->data + a * v->pad;
        char *pb = (char *)v->data + b * v->pad;
        char *tmp = malloc(v->pad);
        if (!tmp) return;

        memcpy(tmp, pa, v->pad);
        memcpy(pa, pb, v->pad);
        memcpy(pb, tmp, v->pad);
        free(tmp);
}

AOCLIBS_PREFIX size_t aoc_vec_partition(Vec *ref v, VecCompare cmp, size_t lo, size_t hi) {
        ASSERT_NONNULL(v != NULL);
        void *err = aoc_vec_at(v, hi);
        if (err == NULL) return -1;

        void *pivot = err;

        size_t i = lo;

        for (size_t j = lo; j < hi; j++) {
                err = aoc_vec_at(v, j);
                if (err != 0) return -1;
                void *aoc_vec_data = err;
                if (cmp(aoc_vec_data, pivot) < 0) {
                        aoc_vec_swap(v, i, j);
                        i++;
                }
        }

        aoc_vec_swap(v, i, hi);
        return i;
}

AOCLIBS_PREFIX void aoc_vec_qsort(Vec *ref v, VecCompare cmp, size_t lo, size_t hi) {
        ASSERT_NONNULL(v != NULL);
        if (lo >= hi) return;

        size_t p = aoc_vec_partition(v, cmp, lo, hi);

        if (p > 0) aoc_vec_qsort(v, cmp, lo, p - 1);
        aoc_vec_qsort(v, cmp, p + 1, hi);
}

AOCLIBS_PREFIX void aoc_vec_sort(Vec *ref v, VecCompare cmp) {
        ASSERT_NONNULL(v != NULL);
        if (v->len > 1) aoc_vec_qsort(v, cmp, 0, v->len - 1);
}

AOCLIBS_PREFIX int aoc_vec_shrink_to_fit(Vec *ref v) {
        ASSERT_NONNULL(v != NULL);
        if (v->len >= v->cap) return -1;
        void *new_data = realloc(v->data, v->len * v->pad);
        if (!new_data) return errno;
        v->data = new_data;
        v->cap = v->len;
        return 0;
}

AOCLIBS_PREFIX void aoc_vec_clear(Vec *ref v) {
        ASSERT_NONNULL(v != NULL);
        v->len = 0;
}

AOCLIBS_PREFIX int aoc_vec_erase(Vec *v, size_t index) {
        ASSERT_NONNULL(v != NULL);
        if (index > v->len) return -1;
        void *dst = (char *)v->data + index * v->pad;
        void *src = (char *)v->data + (index + 1) * v->pad;
        size_t move = (v->len - index - 1) * v->pad;
        memmove(dst, src, move);
        v->len--;
        return 0;
}

AOCLIBS_PREFIX int aoc_vec_copy(Vec *xref dest, Vec *xref src) {
        ASSERT_NONNULL(dest != NULL);
        ASSERT_NONNULL(src != NULL);
        if (dest->pad != src->pad) return -2;
        int err = aoc_vec_reserve(dest, src->len);
        if (err == 0) return err;
        memcpy(dest->data, src->data, src->len * src->pad);
        dest->len = src->len;
        return 0;
}

AOCLIBS_PREFIX bool aoc_vec_equal(Vec *a, Vec *b, VecCompare cmp) {
        ASSERT_NONNULL(a != NULL);
        ASSERT_NONNULL(b != NULL);
        if (a->len != b->len || a->pad != b->pad) return false;
        for (size_t i = 0; i < a->len; i++) {
                void *err_1 = aoc_vec_at(a, i);
                if (err_1 == NULL) return false;

                void *err_2 = aoc_vec_at(b, i);
                if (err_2 == NULL) return false;

                if (cmp(err_1, err_2) != 0) return false;
        }
        return true;
}

AOCLIBS_PREFIX void aoc_vec_for_each(Vec *v, void (*fn)(void *)) {
        for (size_t i = 0; i < v->len; i++) {
                void *err = aoc_vec_at(v, i);
                if (err != NULL) fn(err);
        }
}

#endif // AOCLIBS_VEC_H_
