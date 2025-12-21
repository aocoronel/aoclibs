#include <aoclibs/common.h>
#include <aoclibs/heap/vec.h>
#include <errno.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

eVec werr_vec(Err err, Vec vec) {
        return (eVec){ .err = err, .value = vec };
}

Vec vec_init(usize pad) {
        return (Vec){ .data = NULL, .len = 0, .cap = 0, .pad = pad };
}

void vec_free(Vec *_Nonnull v) {
        ASSERT(v != NULL, "%s", "double free attempt");
        free(v->data);
        *v = (Vec){ 0 };
}

Err vec_reserve(Vec *_Nonnull v, usize cap) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");

        if (cap > SIZE_MAX / v->pad) return ErrMaxCapacityReached;

        void *tmp_ptr = realloc(v->data, cap * v->pad);
        if (!tmp_ptr) return ErrErrno;

        void *new_data = tmp_ptr;

        v->data = new_data;
        v->cap = cap;
        return ok();
}

Err vec_realloc(Vec *_Nonnull v) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");

        usize new_cap = v->cap ? v->cap * 2 : 8;

        if (new_cap > SIZE_MAX / v->pad) return ErrMaxCapacityReached;

        void *new_data = realloc(v->data, new_cap * v->pad);
        if (!new_data) return ErrErrno;

        v->data = new_data;
        v->cap = new_cap;
        return ok();
}

evoid vec_at(Vec *_Nonnull v, usize index) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (index > v->len) return werr_void(ErrIndexNoExist, NULL);
        return werr_void(ok(), (char *)v->data + index * v->pad);
}

Err vec_push(Vec *_Nonnull v, const void *value) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        Err err = vec_reserve(v, v->len + 1);
        if (err.code != Ok) return err;
        char *dest = (char *)v->data + v->len * v->pad;
        memcpy(dest, value, v->pad);
        v->len++;
        return ok();
}

Err vec_pop(Vec *_Nonnull v, void *_Nonnull mut output) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (v->len == 0) return ErrElementIsEmpty;
        v->len--;
        if (output) {
                memcpy(output, (char *)v->data + v->len * v->pad, v->pad);
        }
        return ok();
}

Err vec_insert(Vec *_Nonnull v, usize index, const void *_Nullable value) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (index > v->len) return ErrIndexNoExist;
        Err err = vec_reserve(v, v->len + 1);
        if (err.code != Ok) return err;

        void *dst = (char *)v->data + (index + 1) * v->pad;
        void *src = (char *)v->data + index * v->pad;
        usize move = (v->len - index) * v->pad;

        memmove(dst, src, move);
        memcpy(src, value, v->pad);
        v->len++;
        return ok();
}

void vec_swap(Vec *_Nonnull v, usize a, usize b) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
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

usize vec_partition(Vec *_Nonnull v, VecCompare cmp, usize lo, usize hi) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        evoid err = vec_at(v, hi);
        if (err.err.code != Ok) return -1;

        void *pivot = err.value;

        usize i = lo;

        for (usize j = lo; j < hi; j++) {
                err = vec_at(v, j);
                if (err.err.code != Ok) return -1;
                void *vec_data = err.value;
                if (cmp(vec_data, pivot) < 0) {
                        vec_swap(v, i, j);
                        i++;
                }
        }

        vec_swap(v, i, hi);
        return i;
}

void vec_quicksort(Vec *_Nonnull v, VecCompare cmp, usize lo, usize hi) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (lo >= hi) return;

        usize p = vec_partition(v, cmp, lo, hi);

        if (p > 0) vec_quicksort(v, cmp, lo, p - 1);
        vec_quicksort(v, cmp, p + 1, hi);
}

void vec_sort(Vec *_Nonnull v, VecCompare cmp) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (v->len > 1) vec_quicksort(v, cmp, 0, v->len - 1);
}

Err vec_shrink_to_fit(Vec *_Nonnull v) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (v->len >= v->cap) return ErrNothingToDo;
        void *new_data = realloc(v->data, v->len * v->pad);
        if (!new_data) return ErrErrno;
        v->data = new_data;
        v->cap = v->len;
        return ok();
}

void vec_clear(Vec *_Nonnull v) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        v->len = 0;
}

Err vec_erase(Vec *v, usize index) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (index > v->len) return ErrIndexNoExist;
        void *dst = (char *)v->data + index * v->pad;
        void *src = (char *)v->data + (index + 1) * v->pad;
        usize move = (v->len - index - 1) * v->pad;
        memmove(dst, src, move);
        v->len--;
        return ok();
}

Err vec_resize(Vec *_Nonnull v, usize new_len, const void *_Nullable value) {
        ASSERT(v != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (new_len > v->len) {
                Err err = vec_reserve(v, new_len);
                if (err.code != Ok) return err;
                for (usize i = v->len; i < new_len; i++)
                        memcpy((char *)v->data + i * v->pad, value, v->pad);
        }
        v->len = new_len;
        return ok();
}

Err vec_copy(Vec *_Nonnull restrict mut dest, Vec *_Nonnull restrict src) {
        if (dest->pad != src->pad) return ErrIncompatibleTypes;
        Err err = vec_reserve(dest, src->len);
        if (err.code != Ok) return err;
        memcpy(dest->data, src->data, src->len * src->pad);
        dest->len = src->len;
        return ok();
}

int vec_equal(Vec *a, Vec *b, VecCompare cmp) {
        if (a->len != b->len || a->pad != b->pad) return -1;
        for (usize i = 0; i < a->len; i++) {
                evoid err_1 = vec_at(a, i);
                if (err_1.err.code != Ok) return -1;

                evoid err_2 = vec_at(b, i);
                if (err_2.err.code != Ok) return -1;

                if (cmp(err_1.value, err_2.value) != 0) return -1;
        }
        return 0;
}

void vec_for_each(Vec *v, void (*fn)(void *)) {
        for (usize i = 0; i < v->len; i++) {
                evoid err = vec_at(v, i);
                if (err.err.code == Ok) fn(err.value);
        }
}
