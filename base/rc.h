#ifndef AOCLIBS_RC_H_
#define AOCLIBS_RC_H_

#include "base.h"
#include <alloca.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define array_len(a) sizeof((a)) / sizeof((a[0]))
#define cstr_literal_len(s) array_len((" " s " ")) - sizeof((s)[0])

// --> printf("%s "SLICE_FMT" stuff...", "do", GSLICE(myslice));
#define SLICE_FMT "%.*s"
#define VSLICE(r) ((r).len), ((r).ptr)

typedef struct {
        int len;
        const char *ptr;
} rcslice;

typedef enum {
        RCHeap,
        RCStack,
        RCLiteral,
} RCType;

typedef struct {
        size_t len;
        size_t cap;
        RCType type;
        char *ptr;
} rc; // row char

#ifndef AOCLIBS_RC_NO_ALLOCATOR
#define RC_MALLOC malloc
#define RC_REALLOC realloc
#define RC_FREE free
#else
#ifndef RC_MALLOC
#error "RC_MALLOC not defined!"
#endif
#ifndef RC_REALLOC
#error "RC_REALLOC not defined!"
#endif
#ifndef RC_FREE
#error "RC_FREE not defined!"
#endif
#endif

// Compile-time known RC, which cannot be mutated
#define rcl_new(s)                                                                             \
        (rc) {                                                                                 \
                .ptr = " "(s) " ", .len = cstr_literal_len((s)), .cap = cstr_literal_len((s)), \
                .type = RCLiteral                                                              \
        }

// RC from already allocated char * in the stack/heap
#define rch_bnew(buff, length, capacity)                                          \
        (rc) {                                                                    \
                .ptr = (buff), .len = (length), .cap = (capacity), .type = RCHeap \
        }
#define rcs_bnew(buff)                                                          \
        (rc) {                                                                  \
                .ptr = (buff), .len = 0, .cap = sizeof((buff)), .type = RCStack \
        }

// Allocates a new stack/heap RC
#define rch_new(capacity)                                                                 \
        (rc) {                                                                            \
                .ptr = RC_MALLOC((capacity)), .len = 0, .cap = (capacity), .type = RCHeap \
        }
#define rcs_new(capacity)                                                               \
        (rc) {                                                                          \
                .ptr = alloca((capacity)), .len = 0, .cap = (capacity), .type = RCStack \
        }

#define rc_to_slice(r, start, end) aoc_cstr_to_slice((r.ptr), (start), (end))
#define cstr_to_slice aoc_cstr_to_slice
AOCLIBS_PREFIX rcslice aoc_cstr_to_slice(const char *ref s, size_t start, size_t end) {
        ASSERT_NONNULL(s != NULL);
        return (rcslice){ .ptr = s + start, .len = end - start };
}

#define rc_can_mut aoc_rc_can_mut
AOCLIBS_PREFIX bool aoc_rc_can_mut(const rc *null r) {
        if (!r) return false;
        if (r->type == RCLiteral) return false;
        return true;
}

#define rc_is_null aoc_rc_is_null
AOCLIBS_PREFIX bool aoc_rc_is_null(const rc *null r) {
        return !r || !r->ptr;
}

#define cstr_dup(s, len) aoc_cstr_dup((s), (len) + 1)
AOCLIBS_PREFIX char *xnull aoc_cstr_dup(const char *ref s, const size_t len) {
        ASSERT_NONNULL(s != NULL);
        char *d = RC_MALLOC(len);
        if (!d) return NULL;
        return memcpy(d, s, len);
}

#define rc_dup aoc_rc_dup
AOCLIBS_PREFIX rc aoc_rc_dup(const rc *ref s) {
        ASSERT_NONNULL(rc_is_null(s));
        return (rc){
                .ptr = cstr_dup(s->ptr, s->cap),
                .len = s->len,
                .cap = s->cap,
                .type = RCHeap,
        };
}

#define rc_to_lower(r) aoc_cstr_to_lower((r).ptr, (r).len)
#define cstr_to_lower aoc_cstr_to_lower
AOCLIBS_PREFIX void aoc_cstr_to_lower(char *ref s, const size_t len) {
        ASSERT_NONNULL(s != NULL);
        for (size_t i = 0; i < len; i++) {
                s[i] = tolower(s[i]);
        }
}

#define rc_eq(r1, r2) aoc_cstr_eq((r1).ptr, (r2).ptr)
#define cstr_eq aoc_cstr_eq
AOCLIBS_PREFIX bool aoc_cstr_eq(const char *xref s1, const char *xref s2) {
        if (!s1 || !s2) return s1 == s2;
        while (*s1 && *s2 && *s1 == *s2) {
                s1++;
                s2++;
        }
        return *s1 == *s2;
}

#define rcslice_eq(r1, r2) aoc_cstrn_eq((r1).ptr, (r2).ptr, (r1).len, (r2).len)
#define rcn_eq(r1, r2) aoc_cstrn_eq((r1).ptr, (r2).ptr, (r1).cap, (r2).cap)
#define cstrn_eq aoc_cstrn_eq
AOCLIBS_PREFIX bool aoc_cstrn_eq(const char *xref s1, const char *xref s2, size_t s1_buff,
                                 size_t s2_buff) {
        if (!s1 || !s2) return s1 == s2;
        size_t i = 0;
        while (*s1 && *s2 && s1_buff > i && s2_buff > i && *s1 == *s2) {
                s1++;
                s2++;
                i++;
        }
        return *s1 == *s2;
}

#define rcslice_eq_case(r1, r2) aoc_cstrn_eq((r1).ptr, (r2).ptr, (r1).len, (r2).len)
#define rcn_eq_case(r1, r2) aoc_cstrn_eq((r1).ptr, (r2).ptr, (r1).cap, (r2).cap)
#define cstrn_eq_case aoc_cstrn_eq
AOCLIBS_PREFIX bool aoc_cstrn_eq_case(const char *xref s1, const char *xref s2, size_t s1_buff,
                                      size_t s2_buff) {
        if (!s1 || !s2) return s1 == s2;
        size_t i = 0;
        while (*s1 && *s2 && s1_buff > i && s2_buff > i && tolower(*s1) == tolower(*s2)) {
                s1++;
                s2++;
                i++;
        }
        return tolower(*s1) == tolower(*s2);
}

#define rc_eq_case(r1, r2) cstr_eq_case((r1).ptr, (r2).ptr)
#define cstr_eq_case aoc_cstr_eq_case
AOCLIBS_PREFIX bool aoc_cstr_eq_case(const char *xref s1, const char *xref s2) {
        if (!s1 || !s2) return s1 == s2;
        while (*s1 && *s2 && tolower(*s1) == tolower(*s2)) {
                s1++;
                s2++;
        }
        return tolower(*s1) == tolower(*s2);
}

#define cstr_len aoc_cstr_len
AOCLIBS_PREFIX size_t aoc_cstr_len(const char *null s, const size_t cap) {
        if (s == NULL) return 0;
        const char *s_tmp = memchr(s, 0, cap);
        return s_tmp ? s_tmp - s : cap;
}

#define rc_resize aoc_rc_resize
AOCLIBS_PREFIX int aoc_rc_resize(rc *ref r, size_t cap) {
        ASSERT_NONNULL(rc_is_null(r));
        ASSERT(r->type == RCHeap, "RC is not heap allocated");
        void *tmp = RC_REALLOC(r->ptr, cap);
        if (tmp == NULL) return -1;
        r->ptr = tmp;
        return 0;
}

#define rc_free aoc_rc_free
AOCLIBS_PREFIX void aoc_rc_free(rc *ref r) {
        ASSERT(rc_is_null(r), "double free attempt");
        ASSERT(r->type == RCHeap, "RC is not heap allocated");
        RC_FREE(r->ptr);
        *r = (rc){ 0 };
}

#define rc_erase aoc_rc_erase
AOCLIBS_PREFIX void aoc_rc_erase(rc *ref r) {
        ASSERT(rc_is_null(r), "double free attempt");
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");
        for (size_t i = 0; i < r->cap; i++) {
                r->ptr[i] = '\0';
        }
        if (r->type == RCHeap) rc_free(r);
}

#define rc_clear(r) aoc_rc_clear((r))
AOCLIBS_PREFIX void aoc_rc_clear(rc *ref r) {
        ASSERT_NONNULL(rc_is_null(r));
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");
        r->ptr[0] = '\0';
        r->len = 0;
}

#define rcslice_begins_with(r, begin_len, begin) \
        aoc_cstr_match_pos(VSLICE((r)), (begin_len), (begin), 0)
#define rcslice_ends_with(r, end_len, end) \
        aoc_cstr_match_pos(VSLICE((r)), (end_len), (end), (r).len - (end_len))

#define rc_begins_with(r, begin_len, begin) \
        aoc_cstr_match_pos((r).len, (r).ptr, (begin_len), (begin), 0)
#define rc_ends_with(r, end_len, end) \
        aoc_cstr_match_pos((r).len, (r).ptr, (end_len), (end), (r).len - (end_len))

#define cstr_begins_with(s, s_len, begin_len, begin) \
        aoc_cstr_match_pos((s_len), (s), (begin_len), (begin), 0)
#define cstr_ends_with(s, s_len, end_len, end) \
        aoc_cstr_match_pos((s_len), (s), (end_len), (end), (s_len) - (end_len))
/*
 * Internal
*/
AOCLIBS_PREFIX bool aoc_cstr_match_pos(size_t s_len, const char *xnull s, size_t pattern_len,
                                       const char *xnull pattern, size_t offset) {
        if (!s || !pattern || pattern_len > s_len || offset > s_len - pattern_len) return false;

        size_t i = offset + pattern_len;
        size_t j = pattern_len;

        while (j > 0) {
                i--;
                j--;
                if (s[i] != pattern[j]) return false;
        }
        return true;
}

#define rc_overwrite(s1, s2) aoc_rc_copy((s1), (s2), 0)
#define rc_cat(s1, s2) aoc_rc_copy((s1), (s2), (s1).len)
#define rc_append(s1, s2) aoc_rc_copy((s1), (s2), (s1).len + 1)
AOCLIBS_PREFIX int aoc_rc_copy(rc *xref r1, const rc *xref r2, const size_t r1_offset) {
        ASSERT_NONNULL(rc_is_null(r1));
        ASSERT_NONNULL(rc_is_null(r2));
        ASSERT(r1->type != RCLiteral, "attempt to modify RC literal");
        if (!rc_can_mut(r1) || rc_is_null(r2)) return -1;

        size_t avail = r1->cap - r1_offset;
        size_t needed = r2->len + 1;

        if (needed > avail) {
                if (r1->type != RCHeap) return -1;

                if (rc_resize(r1, r1_offset + needed) != 0) return -1;
        }

        memcpy(r1->ptr + r1_offset, r2->ptr, r2->len);
        r1->ptr[r1_offset + r2->len] = '\0';

        r1->len = r1_offset + r2->len;

        return 0;
}

#define rc_push aoc_rc_push
AOCLIBS_PREFIX int aoc_rc_push(rc *ref r, char c) {
        ASSERT_NONNULL(rc_is_null(r));
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");

        size_t needed = r->len + 2;
        if (needed > r->cap) {
                if (rc_resize(r, needed) != 0) return -1;
        }

        ((char *)r->ptr)[r->len] = c;
        ((char *)r->ptr)[r->len + 1] = '\0';
        r->len++;
        return 0;
}

#define rc_pop aoc_rc_pop
AOCLIBS_PREFIX int aoc_rc_pop(rc *ref r) {
        ASSERT_NONNULL(rc_is_null(r));
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");

        if (r->len == 0) return -1;

        r->len--;
        ((char *)r->ptr)[r->len] = '\0';
        return 0;
}

#define rc_drop aoc_rc_drop
AOCLIBS_PREFIX int aoc_rc_drop(rc *ref r, size_t index) {
        ASSERT_NONNULL(rc_is_null(r));
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");
        if (index >= r->len) return -1;

        char *ptr = (char *)r->ptr;

        memmove(&ptr[index], &ptr[index + 1], r->len - index);

        r->len--;
        ptr[r->len] = '\0';

        return 0;
}

#define cstr_copy aoc_cstr_copy
#define cstr_overwrite(s1, s2, s2_len) cstr_copy((s1), (s2), 0, s2_len)
#define cstr_overwrite_c(s1, s2) cstr_copy((s1), (s2), 0, cstr_len_c(s2))
#define cstr_cat(s1, s2) cstr_copy((s1), (s2), (s1).len, cstr_len((s2)))
#define cstr_cat_c(s1, s2) cstr_copy((s1), (s2), (s1).len, cstr_len_c((s2)))
#define cstr_append(s1, s2) cstr_copy((s1), (s2), ((s1).len + 1), cstr_len((s2)))
#define cstr_append_c(s1, s2) cstr_copy((s1), (s2), ((s1).len + 1), cstr_len_c((s2)))
#define rc_null_terminate(s) cstr_copy((s), "\0", (*(s).len), 1)
AOCLIBS_PREFIX int aoc_cstr_copy(rc *xref r, const char *xref s, const size_t r_offset,
                                 size_t s_len) {
        ASSERT_NONNULL(rc_is_null(r));
        ASSERT_NONNULL(s != NULL);
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");
        if (!rc_can_mut(r) || !s) return -1;

        size_t avail = r->cap - r_offset;
        size_t needed = s_len + 1;

        if (needed > avail) {
                if (r->type != RCHeap) return -1;
                if (rc_resize(r, r_offset + needed) != 0) return -1;
        }

        memcpy(r->ptr + r_offset, s, s_len);
        r->ptr[r_offset + s_len] = '\0';

        r->len = r_offset + s_len;

        return 0;
}

#ifndef AOCLIBS_RC_NO_STDIO
#define cstr_copy_fmt aoc_cstr_copy_fmt
AOCLIBS_PREFIX int aoc_cstr_copy_fmt(rc *xref r, const char *xref fmt, ...) {
        ASSERT_NONNULL(rc_is_null(r));
        ASSERT_NONNULL(fmt != NULL);
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");
        if (!rc_can_mut(r) || !fmt) return -1;

        int needed_len = 0, allocated_len = 0;

        va_list args;
        va_start(args, fmt);
        needed_len = vsnprintf(r->ptr, 0, fmt, args);
        va_end(args);

        if (needed_len < 0) return -1;

        if ((size_t)needed_len >= r->cap) {
                if (r->type == RCHeap)
                        if (rc_resize(r, needed_len + 1) != 0) return -1;
                va_start(args, fmt);
                allocated_len = vsnprintf(r->ptr, r->cap, fmt, args);
                va_end(args);
                if (allocated_len < 0) return -1;
        }

        r->len = allocated_len;
        return 0;
}
#endif

#define cstr_find_delim aoc_cstr_find_delim
/*
 * Internal
*/
AOCLIBS_PREFIX bool aoc_cstr_find_delim(const char *xnull s, size_t s_len, const char *xnull delim,
                                        size_t delim_len, size_t pos, size_t j) {
        if (!s || !delim) return false;
        if (j == delim_len) return true;
        if (pos + j >= s_len) return false;
        if (s[pos + j] != delim[j]) return false;
        return cstr_find_delim(s, s_len, delim, delim_len, pos, j + 1);
}

#define rc_chr_cstr aoc_rc_chr_cstr
AOCLIBS_PREFIX size_t aoc_rc_chr_cstr(const rc *xref r, const char *xref delim,
                                      const size_t delim_len) {
        ASSERT_NONNULL(rc_is_null(r));
        ASSERT_NONNULL(delim != NULL);

        for (size_t i = 0; i < r->len; i++) {
                if (cstr_find_delim(r->ptr, r->len, delim, delim_len, i, 0)) {
                        return (size_t)i;
                }
        }

        return SIZE_MAX;
}

#define rc_chr aoc_rc_chr
AOCLIBS_PREFIX size_t aoc_rc_chr(const rc *ref r, char delim) {
        ASSERT_NONNULL(rc_is_null(r));

        for (size_t i = 0; i < r->len; i++) {
                if (r->ptr[i] == delim) return i;
        }

        return SIZE_MAX;
}
#define rc_tok_cstr aoc_rc_tok_cstr
AOCLIBS_PREFIX const char *null aoc_rc_tok_cstr(const rc *xref r, const char *xref delim,
                                                const size_t delim_len) {
        ASSERT_NONNULL(rc_is_null(r));
        ASSERT_NONNULL(delim != NULL);

        size_t pos = rc_chr_cstr(r, delim, delim_len);
        if (pos == SIZE_MAX) return NULL;

        return r->ptr + pos + delim_len;
}

#define rc_tok aoc_rc_tok
AOCLIBS_PREFIX const char *null aoc_rc_tok(const rc *ref r, char delim) {
        ASSERT_NONNULL(rc_is_null(r));

        size_t pos = rc_chr(r, delim);
        if (pos == SIZE_MAX) return NULL;

        return r->ptr + pos + 1;
}

#define cstr_trim_whitespace aoc_cstr_trim_whitespace
AOCLIBS_PREFIX size_t aoc_cstr_trim_whitespace(char *ref s, const size_t len) {
        ASSERT_NONNULL(s != NULL);

        if (len == 0) return 0;

        size_t i = len;
        while (i > 0 && isspace((unsigned char)s[i - 1])) {
                i--;
        }
        s[i] = '\0';
        return len;
}

#define rc_trim_whitespace aoc_rc_trim_whitespace
AOCLIBS_PREFIX void aoc_rc_trim_whitespace(rc *ref r) {
        ASSERT_NONNULL(rc_is_null(r));
        ASSERT(r->type != RCLiteral, "attempt to modify RC literal");

        size_t len = cstr_trim_whitespace(r->ptr, r->len);
        r->len = len;
}

#define cstr_to_double aoc_cstr_to_double
AOCLIBS_PREFIX double aoc_cstr_to_double(const char *ref s, const double _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        double val = strtod(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

#define cstr_to_bool aoc_cstr_to_bool
AOCLIBS_PREFIX bool aoc_cstr_to_bool(const char *ref s, const bool _default) {
        ASSERT_NONNULL(s != NULL);
        if (aoc_cstr_eq_case(s, "true") || aoc_cstr_eq(s, "1")) return true;
        if (aoc_cstr_eq_case(s, "false") || aoc_cstr_eq(s, "0")) return false;
        return _default;
}

#define cstr_to_float aoc_cstr_to_float
AOCLIBS_PREFIX float aoc_cstr_to_float(const char *ref s, const float _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        float val = strtof(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

#define cstr_to_long aoc_cstr_to_long
AOCLIBS_PREFIX long aoc_cstr_to_long(const char *ref s, const long _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        long val = strtol(s, &endptr, 10);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

#endif // AOCLIBS_RC_H_
