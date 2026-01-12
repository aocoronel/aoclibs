#include "assert.h"
#include "attributes.h"
#include "vec.h"
#include <errno.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vec aoc_vec_init(size_t pad) {
        return (Vec){ .data = NULL, .len = 0, .cap = 0, .pad = pad };
}

void aoc_vec_free(Vec *ref v) {
        ASSERT(v != NULL, "%s", "double free attempt");
        free(v->data);
        *v = (Vec){ 0 };
}

int aoc_vec_reserve(Vec *ref v, size_t new_len) {
        ASSERT_NONNULL(v != NULL);

        if (new_len > SIZE_MAX / v->pad) return -1;

        void *tmp_ptr = realloc(v->data, new_len * v->pad);
        if (!tmp_ptr) return errno;

        void *new_data = tmp_ptr;

        v->data = new_data;
        v->cap = new_len;
        return 0;
}

int aoc_vec_realloc(Vec *ref v) {
        ASSERT_NONNULL(v != NULL);

        size_t new_cap = v->cap ? v->cap * AOCLIBS_VECTOR_GROWTH_FACTOR : 8;

        if (new_cap > SIZE_MAX / v->pad) return -1;

        void *new_data = realloc(v->data, new_cap * v->pad);
        if (!new_data) return errno;

        v->data = new_data;
        v->cap = new_cap;
        return 0;
}

void *aoc_vec_at(Vec *ref v, size_t index) {
        ASSERT_NONNULL(v != NULL);
        if (index > v->len) return NULL;

        return (char *)v->data + index * v->pad;
}

int aoc_vec_push(Vec *ref v, const void *value) {
        ASSERT_NONNULL(v != NULL);
        int err = aoc_vec_reserve(v, v->len + 1);
        if (err != 0) return err;
        char *dest = (char *)v->data + v->len * v->pad;
        memcpy(dest, value, v->pad);
        v->len++;
        return 0;
}

int aoc_vec_pop(Vec *ref v, void *ref output) {
        ASSERT_NONNULL(v != NULL);
        if (v->len == 0) return -1;
        v->len--;
        if (output) {
                memcpy(output, (char *)v->data + v->len * v->pad, v->pad);
        }
        return 0;
}

int aoc_vec_insert(Vec *ref v, size_t index, const void *ref value) {
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

void aoc_vec_swap(Vec *ref v, size_t a, size_t b) {
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

size_t aoc_vec_partition(Vec *ref v, VecCompare cmp, size_t lo, size_t hi) {
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

void aoc_vec_qsort(Vec *ref v, VecCompare cmp, size_t lo, size_t hi) {
        ASSERT_NONNULL(v != NULL);
        if (lo >= hi) return;

        size_t p = aoc_vec_partition(v, cmp, lo, hi);

        if (p > 0) aoc_vec_qsort(v, cmp, lo, p - 1);
        aoc_vec_qsort(v, cmp, p + 1, hi);
}

void aoc_vec_sort(Vec *ref v, VecCompare cmp) {
        ASSERT_NONNULL(v != NULL);
        if (v->len > 1) aoc_vec_qsort(v, cmp, 0, v->len - 1);
}

int aoc_vec_shrink_to_fit(Vec *ref v) {
        ASSERT_NONNULL(v != NULL);
        if (v->len >= v->cap) return -1;
        void *new_data = realloc(v->data, v->len * v->pad);
        if (!new_data) return errno;
        v->data = new_data;
        v->cap = v->len;
        return 0;
}

void aoc_vec_clear(Vec *ref v) {
        ASSERT_NONNULL(v != NULL);
        v->len = 0;
}

int aoc_vec_erase(Vec *v, size_t index) {
        ASSERT_NONNULL(v != NULL);
        if (index > v->len) return -1;
        void *dst = (char *)v->data + index * v->pad;
        void *src = (char *)v->data + (index + 1) * v->pad;
        size_t move = (v->len - index - 1) * v->pad;
        memmove(dst, src, move);
        v->len--;
        return 0;
}

int aoc_vec_copy(Vec *xref dest, Vec *xref src) {
        ASSERT_NONNULL(dest != NULL);
        ASSERT_NONNULL(src != NULL);
        if (dest->pad != src->pad) return -2;
        int err = aoc_vec_reserve(dest, src->len);
        if (err == 0) return err;
        memcpy(dest->data, src->data, src->len * src->pad);
        dest->len = src->len;
        return 0;
}

bool aoc_vec_equal(Vec *a, Vec *b, VecCompare cmp) {
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

void aoc_vec_for_each(Vec *v, void (*fn)(void *)) {
        for (size_t i = 0; i < v->len; i++) {
                void *err = aoc_vec_at(v, i);
                if (err != NULL) fn(err);
        }
}
