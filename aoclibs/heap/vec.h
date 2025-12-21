#ifndef AOCLIBS_VEC_H
#define AOCLIBS_VEC_H

#include <aoclibs/common.h>
#include <stdalign.h>

typedef int (*VecCompare)(const void *a, const void *b);

typedef struct {
        void *data;
        usize len;
        usize cap;
        usize pad; // Size of each element in bytes
} Vec;

typedef struct {
        Err err;
        Vec value;
} eVec;

eVec werr_vec(Err err, Vec vec);
Vec vec_init(usize pad);
void vec_free(Vec *_Nonnull v);
Err vec_reserve(Vec *_Nonnull v, usize cap) FN_WARN_UNUSED;
Err vec_realloc(Vec *_Nonnull v) FN_WARN_UNUSED;
evoid vec_at(Vec *_Nonnull v, usize index) FN_WARN_UNUSED;
Err vec_push(Vec *_Nonnull v, const void *value) FN_WARN_UNUSED;
Err vec_pop(Vec *_Nonnull v, void *_Nonnull mut output) FN_WARN_UNUSED;
Err vec_insert(Vec *_Nonnull v, usize index,
               const void *_Nullable value) FN_WARN_UNUSED;
void vec_swap(Vec *_Nonnull v, usize a, usize b);
usize vec_partition(Vec *_Nonnull v, VecCompare cmp, usize lo, usize hi);
void vec_quicksort(Vec *_Nonnull v, VecCompare cmp, usize lo, usize hi);
void vec_sort(Vec *_Nonnull v, VecCompare cmp);
Err vec_shrink_to_fit(Vec *_Nonnull v) FN_WARN_UNUSED;
void vec_clear(Vec *_Nonnull v);
Err vec_erase(Vec *v, usize index) FN_WARN_UNUSED;
Err vec_resize(Vec *_Nonnull v, usize new_len,
               const void *_Nullable value) FN_WARN_UNUSED;
Err vec_copy(Vec *_Nonnull restrict mut dest,
             Vec *_Nonnull restrict src) FN_WARN_UNUSED;
int vec_equal(Vec *a, Vec *b, VecCompare cmp);
void vec_for_each(Vec *v, void (*fn)(void *));

#endif
