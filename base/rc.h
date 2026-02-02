#ifndef AOCLIBS_RC_H_
#define AOCLIBS_RC_H_

#include "base.h"
#include "cstr.h"
#include <alloca.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Separate implementation and definitions
// TODO: Strip prefixes
// TODO: Port all cstr specific code to cstr.h

// --> printf("%s "SLICE_FMT" stuff...", "do", VSLICE(myslice));
#define SLICE_FMT "%.*s"
#define VSLICE(r) ((r).len), ((r).data)

typedef enum {
        RCHeap,
        RCStack,
        RCLiteral,
} RCType;

// Row Char
// This is Dynamic Array compatible. This implementation is suppose to give you a 'String' type
// which provides safer operations. It can work with string literals, stack allocated and heap
// allocated strings. If you decide to use this like an Dynamic Array, make sure to just use it
// with a heap RC.
typedef struct {
        RCType type;
        size_t cap;
        size_t len;
        char *data;
} rc;

#ifndef RC_MALLOC
#define RC_MALLOC malloc
#endif
#ifndef RC_REALLOC
#define RC_REALLOC realloc
#endif
#ifndef RC_FREE
#define RC_FREE free
#endif

// In short:
// rcl --> row char literal
// rcs --> row char stack
// rch --> row char heap

// Compile-time known RC, which cannot be mutated
#define rcl_new(s)                                                                              \
        (rc) {                                                                                  \
                .data = " "(s) " ", .len = cstr_literal_len((s)), .cap = cstr_literal_len((s)), \
                .type = RCLiteral                                                               \
        }

// RC from already allocated char * in the stack/heap
#define rch_bnew(buff, length, capacity)                                           \
        (rc) {                                                                     \
                .data = (buff), .len = (length), .cap = (capacity), .type = RCHeap \
        }
#define rcs_bnew(buff, length)                                                        \
        (rc) {                                                                        \
                .data = (buff), .len = length, .cap = sizeof((buff)), .type = RCStack \
        }

// Allocates a new stack/heap RC
#define rch_new(capacity)                                                                  \
        (rc) {                                                                             \
                .data = RC_MALLOC((capacity)), .len = 0, .cap = (capacity), .type = RCHeap \
        }
#define rch_new_a(allocator, capacity)                                                     \
        (rc) {                                                                             \
                .data = allocator((capacity)), .len = 0, .cap = (capacity), .type = RCHeap \
        }
#define rcs_new(capacity)                                                                \
        (rc) {                                                                           \
                .data = alloca((capacity)), .len = 0, .cap = (capacity), .type = RCStack \
        }

#define rc_can_mut aoc_rc_can_mut
AOCLIBS_PREFIX bool aoc_rc_can_mut(const rc *null r) {
        if (!r) return false;
        if (r->type == RCLiteral) return false;
        return true;
}

#define rc_is_null aoc_rc_is_null
AOCLIBS_PREFIX bool aoc_rc_is_null(const rc *null r) {
        return !r || !r->data;
}

#define rc_dup aoc_rc_dup
AOCLIBS_PREFIX rc aoc_rc_dup(const rc *ref r) {
        ASSERT_NONNULL(aoc_rc_is_null(r));
        return (rc){
                .data = aoc_cstr_dup(r->data, r->cap),
                .len = r->len,
                .cap = r->cap,
                .type = RCHeap,
        };
}

#define rc_to_lower(r) aoc_cstr_to_lower((r)->data)
#define rcn_to_lower(r) aoc_cstr_to_lower((r)->data, (r)->len)

#define rc_eq(r, pattern) aoc_cstr_eq((r)->data, (pattern))
#define rcn_eq(r, pattern, pattern_len) aoc_cstrn_eq((r)->data, (r)->len, (pattern), (pattern_len))

#define rc_eq_case(r, pattern) aoc_cstr_eq_case((r)->data, (pattern))
#define rcn_eq_case(r, pattern, pattern_len) \
        aoc_cstrn_eq_case((r)->data, (r)->len, (pattern), (pattern_len))

// TODO: Use Dynamic Arrays to do these operations
#define rc_resize(r, cap) aoc_rc_resize(RC_REALLOC, (r), (cap))
#define rc_resize_a aoc_rc_resize
AOCLIBS_PREFIX int aoc_rc_resize(aoc_realloc_t allocator, rc *ref r, size_t cap) {
        ASSERT_NONNULL(aoc_rc_is_null(r));
        ASSERT(r->type == RCHeap, "RC is not heap allocated");
        void *tmp = allocator(r->data, cap);
        if (tmp == NULL) return -1;
        r->data = tmp;
        return 0;
}

#define rc_free(r) aoc_rc_free(RC_FREE, (r))
#define rc_free_a aoc_rc_free
AOCLIBS_PREFIX void aoc_rc_free(aoc_free_t _free, rc *ref r) {
        ASSERT(aoc_rc_is_null(r), "double free attempt");
        ASSERT(r->type == RCHeap, "RC is not heap allocated");
        _free(r->data);
        *r = (rc){ 0 };
}

#define rc_erase(r) aoc_rc_erase(RC_FREE, (r))
#define rc_erase_a aoc_rc_erase
AOCLIBS_PREFIX void aoc_rc_erase(aoc_free_t _free, rc *ref r) {
        ASSERT(aoc_rc_is_null(r), "double free attempt");
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");
        for (size_t i = 0; i < r->cap; i++) {
                r->data[i] = '\0';
        }
        if (r->type == RCHeap) aoc_rc_free(_free, r);
}

#define rc_clear(r) aoc_rc_clear((r))
AOCLIBS_PREFIX void aoc_rc_clear(rc *ref r) {
        ASSERT_NONNULL(aoc_rc_is_null(r));
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");
        r->data[0] = '\0';
        r->len = 0;
}

#define rc_begins_with(r, begin_len, begin) \
        aoc_cstr_match_pos((r).len, (r).data, (begin_len), (begin), 0)
#define rc_ends_with(r, end_len, end) \
        aoc_cstr_match_pos((r).len, (r).data, (end_len), (end), (r).len - (end_len))

#define aoc_rc_begins_with(r, begin, begin_len) \
        aoc_cstrn_eq((r)->data, (r)->len, (begin), (begin_len))
#define aoc_rc_ends_with(r, end, end_len) \
        aoc_cstrn_eq((r)->data + (r)->len, (r)->len, (end), (end_len))

#define rc_drop aoc_rc_drop
AOCLIBS_PREFIX int aoc_rc_drop(rc *ref r, size_t index) {
        ASSERT_NONNULL(aoc_rc_is_null(r));
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");
        if (index >= r->len) return -1;

        char *data = (char *)r->data;

        memmove(&data[index], &data[index + 1], r->len - index);

        r->len--;
        data[r->len] = '\0';

        return 0;
}

#endif // AOCLIBS_RC_H_
