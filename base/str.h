#ifndef AOCLIBS_STRING_H_
#define AOCLIBS_STRING_H_

#include "assert.h"
#include "attributes.h"
#include <alloca.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define string_literal_len(s) ((sizeof((s)) / sizeof((s)[0])) - sizeof((s)[0]))
#define array_len(a) sizeof(a) / sizeof(a[0])

#define StrError ((size_t)-1)

typedef enum {
        StringHeap,
        StringStack,
        StringLiteral,
} StringType;

#define get_slice(s) (s.len), (s.slice)
typedef struct {
        const char *slice;
        int len;
} sslice;

typedef struct {
        char *str;
        size_t len;
        size_t cap;
        StringType type;
} str;

#define str_to_slice(s, x, y) cstr_to_slice((s.str), (x), (y))
AOCLIBS_PREFIX sslice cstr_to_slice(const char *ref s, size_t start, size_t end);

AOCLIBS_PREFIX bool str_can_mut(const str *null s);

AOCLIBS_PREFIX bool str_is_null(const str *null s);

AOCLIBS_PREFIX bool str_is_null_assert(const str *null s);

#define cstr_dup(s, len) _cstr_dup((s), (len + 1))
AOCLIBS_PREFIX char *xnull _cstr_dup(const char *ref s, const size_t len);

AOCLIBS_PREFIX str str_dup(const str *ref s);

AOCLIBS_PREFIX void cstr_to_lower(char *ref s, const size_t len);

#define str_eq(s1, s2) cstr_eq((s1.str), (s2.str))
#define str_eq_case(s1, s2) cstr_eq_case((s1.str), (s2.str))

AOCLIBS_PREFIX bool cstr_eq(const char *xref s1, const char *xref s2);

AOCLIBS_PREFIX bool cstr_eq_case(const char *xref s1, const char *xref s2);

#define cstr_len_comptime(s) ((sizeof((" " s " ")) / sizeof((s)[0])) - sizeof((s)[0]))

AOCLIBS_PREFIX size_t cstr_len(const char *null s, const size_t buff);

#define str_new_stack(cap) _str_new_stack(alloca((cap)), cap)
AOCLIBS_PREFIX str _str_new_stack(char *ref s, const size_t cap);

#define str_new_comptime(s) _str_new_comptime((" " s " "), cstr_len_comptime(s))
AOCLIBS_PREFIX str _str_new_comptime(char *ref s, const size_t len);

#define str_new_heap(cap) _str_new_heap(malloc((cap)), (cap))
AOCLIBS_PREFIX str _str_new_heap(char *null s, const size_t cap);

AOCLIBS_PREFIX int str_resize(str *ref s, size_t cap);

AOCLIBS_PREFIX void str_free(str *ref s);

AOCLIBS_PREFIX void str_free_array(str *ref s[], size_t len);

AOCLIBS_PREFIX void str_erase(str *ref s);

AOCLIBS_PREFIX void str_clear(str *ref s);

#define cstr_begins_with(s, begin, begin_len, s_len) \
        cstr_match_pos((s), (begin), (begin_len), (s_len), 0)
#define cstr_ends_with(s, end, end_len, s_len) \
        cstr_match_pos((s), (end), (end_len), (s_len), (s_len) - (end_len))
AOCLIBS_PREFIX bool cstr_match_pos(const char *xnull s, const char *xnull pattern,
                                   size_t pattern_len, size_t s_len, size_t offset);

#define str_overwrite(s1, s2) str_copy((s1), (s2), 0)
#define str_cat(s1, s2) str_copy((s1), (s2), s1.len)
#define str_append(s1, s2) str_copy((s1), (s2), (s1.len + 1))
AOCLIBS_PREFIX int str_copy(str *xref s1, const str *xref s2, const size_t s1_offset);

AOCLIBS_PREFIX int str_push(str *ref s, char c);

AOCLIBS_PREFIX int str_pop(str *ref s);

AOCLIBS_PREFIX int str_drop(str *ref s, size_t index);

#define cster_overwrite(s1, s2, s2_len) cstr_copy((s1), (s2), 0, s2_len)
#define cster_overwrite_comptime(s1, s2) cstr_copy((s1), (s2), 0, cstr_len_comptime(s2))
#define cstr_cat(s1, s2) cstr_copy((s1), (s2), s1.len, cstr_len((s2)))
#define cstr_cat_comptime(s1, s2) cstr_copy((s1), (s2), s1.len, cstr_len_comptime((s2)))
#define cstr_append(s1, s2) cstr_copy((s1), (s2), (s1.len + 1), cstr_len((s2)))
#define cstr_append_comptime(s1, s2) cstr_copy((s1), (s2), (s1.len + 1), cstr_len_comptime((s2)))
#define str_null_terminate(s) cstr_copy((s), "\0", (*s.len), 1)
AOCLIBS_PREFIX int cstr_copy(str *xref s1, const char *xref s2, const size_t s1_offset,
                             size_t s2_len);

AOCLIBS_PREFIX int cstr_copy_fmt(str *xref s, const char *xref fmt, ...);

AOCLIBS_PREFIX bool cstr_find_delim(const char *xnull s, size_t s_len, const char *xnull delim,
                                    size_t delim_len, size_t pos, size_t j);

AOCLIBS_PREFIX size_t str_chr_cstr(const str *xref s, const char *xref delim,
                                   const size_t delim_len);

AOCLIBS_PREFIX size_t str_chr(const str *ref s, char delim);

AOCLIBS_PREFIX const char *null str_tok_cstr(const str *xref s, const char *xref delim,
                                             const size_t delim_len);

AOCLIBS_PREFIX const char *null str_tok(const str *ref s, char delim);

AOCLIBS_PREFIX size_t cstr_trim_whitespace(char *ref s, const size_t len);

AOCLIBS_PREFIX void str_trim_whitespace(str *ref s);

AOCLIBS_PREFIX double cstr_to_double(const char *ref s, const double _default);

AOCLIBS_PREFIX bool cstr_to_bool(const char *ref s, const bool _default);

AOCLIBS_PREFIX float cstr_to_float(const char *ref s, const float _default);

AOCLIBS_PREFIX long cstr_to_long(const char *ref s, const long _default);

AOCLIBS_PREFIX sslice cstr_to_slice(const char *ref s, size_t start, size_t end) {
        ASSERT_NONNULL(s != NULL);
        return (sslice){ .slice = s + start, .len = end - start };
}

AOCLIBS_PREFIX bool str_can_mut(const str *null s) {
        if (!s) return false;
        if (s->type == StringLiteral) return false;
        return true;
}

AOCLIBS_PREFIX bool str_is_null(const str *null s) {
        return !s || !s->str || s->len == 0;
}

AOCLIBS_PREFIX bool str_is_null_assert(const str *null s) {
        return !(!s || !s->str);
}

AOCLIBS_PREFIX char *xnull _cstr_dup(const char *ref s, const size_t len) {
        ASSERT_NONNULL(s != NULL);
        char *d = malloc(len);
        if (!d) return NULL;
        return memcpy(d, s, len);
}

AOCLIBS_PREFIX str str_dup(const str *ref s) {
        ASSERT_NONNULL(str_is_null_assert(s));
        ASSERT(s->type == StringHeap, "string is not heap allocated");
        return (str){
                .str = _cstr_dup(s->str, s->cap),
                .len = s->len,
                .cap = s->cap,
                .type = StringHeap,
        };
}

AOCLIBS_PREFIX void cstr_to_lower(char *ref s, const size_t len) {
        ASSERT_NONNULL(s != NULL);
        for (size_t i = 0; i < len; i++) {
                s[i] = tolower(s[i]);
        }
}

AOCLIBS_PREFIX bool cstr_eq(const char *xref s1, const char *xref s2) {
        if (!s1 || !s2) return s1 == s2;
        while (*s1 && *s2 && *s1 == *s2) {
                s1++;
                s2++;
        }
        return *s1 == *s2;
}

AOCLIBS_PREFIX bool cstr_eq_case(const char *xref s1, const char *xref s2) {
        if (!s1 || !s2) return s1 == s2;
        while (*s1 && *s2 && tolower(*s1) == tolower(*s2)) {
                s1++;
                s2++;
        }
        return tolower(*s1) == tolower(*s2);
}

AOCLIBS_PREFIX size_t cstr_len(const char *null s, const size_t buff) {
        if (s == NULL) return 0;
        const char *s_tmp = memchr(s, 0, buff);
        return s_tmp ? s_tmp - s : buff;
}

AOCLIBS_PREFIX str _str_new_stack(char *ref s, const size_t cap) {
        ASSERT_NONNULL(s != NULL);
        return (str){ .str = s, .len = 0, .cap = cap, .type = StringStack };
}

AOCLIBS_PREFIX str _str_new_comptime(char *ref s, const size_t len) {
        ASSERT_NONNULL(s != NULL);
        return (str){ .str = s, .len = 0, .cap = len, .type = StringLiteral };
}

AOCLIBS_PREFIX str _str_new_heap(char *null s, const size_t cap) {
        return (str){
                .str = s,
                .len = 0,
                .cap = cap,
                .type = StringHeap,
        };
}

AOCLIBS_PREFIX int str_resize(str *ref s, size_t cap) {
        ASSERT_NONNULL(str_is_null_assert(s));
        ASSERT(s->type == StringHeap, "string is not heap allocated");
        void *tmp = realloc(s->str, cap);
        if (tmp == NULL) return -1;
        s->str = tmp;
        return 0;
}

AOCLIBS_PREFIX void str_free(str *ref s) {
        ASSERT(str_is_null_assert(s), "double free attempt");
        ASSERT(s->type == StringHeap, "string is not heap allocated");
        free(s->str);
        *s = (str){ 0 };
}

AOCLIBS_PREFIX void str_free_array(str *ref s[], size_t len) {
        for (size_t i = 0; i < len; i++) {
                str_free(s[i]);
        }
}

AOCLIBS_PREFIX void str_erase(str *ref s) {
        ASSERT(str_is_null_assert(s), "double free attempt");
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");
        for (size_t i = 0; i < s->cap; i++) {
                s->str[i] = '\0';
        }
        if (s->type == StringHeap) str_free(s);
}

AOCLIBS_PREFIX void str_clear(str *ref s) {
        ASSERT_NONNULL(str_is_null_assert(s));
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");
        s->str[0] = '\0';
        s->len = 0;
}

AOCLIBS_PREFIX bool cstr_match_pos(const char *xnull s, const char *xnull pattern,
                                   size_t pattern_len, size_t s_len, size_t offset) {
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

AOCLIBS_PREFIX int str_copy(str *xref s1, const str *xref s2, const size_t s1_offset) {
        ASSERT_NONNULL(str_is_null_assert(s1));
        ASSERT_NONNULL(str_is_null_assert(s2));
        ASSERT(s1->type != StringLiteral, "attempt to modify string literal");
        if (!str_can_mut(s1) || str_is_null_assert(s2)) return -1;

        size_t avail = s1->cap - s1_offset;
        size_t needed = s2->len + 1;

        if (needed > avail) {
                if (s1->type != StringHeap) return -1;

                if (str_resize(s1, s1_offset + needed) != 0) return -1;
        }

        memcpy(s1->str + s1_offset, s2->str, s2->len);
        s1->str[s1_offset + s2->len] = '\0';

        s1->len = s1_offset + s2->len;

        return 0;
}

AOCLIBS_PREFIX int str_push(str *ref s, char c) {
        ASSERT_NONNULL(str_is_null_assert(s));
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");

        size_t needed = s->len + 2;
        if (needed > s->cap) {
                if (str_resize(s, needed) != 0) return -1;
        }

        ((char *)s->str)[s->len] = c;
        ((char *)s->str)[s->len + 1] = '\0';
        s->len++;
        return 0;
}

AOCLIBS_PREFIX int str_pop(str *ref s) {
        ASSERT_NONNULL(str_is_null_assert(s));
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");

        if (s->len == 0) return -1;

        s->len--;
        ((char *)s->str)[s->len] = '\0';
        return 0;
}

AOCLIBS_PREFIX int str_drop(str *ref s, size_t index) {
        ASSERT_NONNULL(str_is_null_assert(s));
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");
        if (index >= s->len) return -1;

        char *ptr = (char *)s->str;

        memmove(&ptr[index], &ptr[index + 1], s->len - index);

        s->len--;
        ptr[s->len] = '\0';

        return 0;
}

AOCLIBS_PREFIX int cstr_copy(str *xref s1, const char *xref s2, const size_t s1_offset,
                             size_t s2_len) {
        ASSERT_NONNULL(str_is_null_assert(s1));
        ASSERT_NONNULL(s2 != NULL);
        ASSERT(s1->type != StringLiteral, "attempt to modify string literal");
        if (!str_can_mut(s1) || !s2) return -1;

        size_t avail = s1->cap - s1_offset;
        size_t needed = s2_len + 1;

        if (needed > avail) {
                if (s1->type != StringHeap) return -1;
                if (str_resize(s1, s1_offset + needed) != 0) return -1;
        }

        memcpy(s1->str + s1_offset, s2, s2_len);
        s1->str[s1_offset + s2_len] = '\0';

        s1->len = s1_offset + s2_len;

        return 0;
}

AOCLIBS_PREFIX int cstr_copy_fmt(str *xref s, const char *xref fmt, ...) {
        ASSERT_NONNULL(str_is_null_assert(s));
        ASSERT_NONNULL(fmt != NULL);
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");
        if (!str_can_mut(s) || !fmt) return -1;

        int needed_len = 0, allocated_len = 0;

        va_list args;
        va_start(args, fmt);
        needed_len = vsnprintf(s->str, 0, fmt, args);
        va_end(args);

        if (needed_len < 0) return -1;

        if ((size_t)needed_len >= s->cap) {
                if (s->type == StringHeap)
                        if (str_resize(s, needed_len + 1) != 0) return -1;
                va_start(args, fmt);
                allocated_len = vsnprintf(s->str, s->cap, fmt, args);
                va_end(args);
                if (allocated_len < 0) return -1;
        }

        s->len = allocated_len;
        return 0;
}

AOCLIBS_PREFIX bool cstr_find_delim(const char *xnull s, size_t s_len, const char *xnull delim,
                                    size_t delim_len, size_t pos, size_t j) {
        if (!s || !delim) return false;
        if (j == delim_len) return true;
        if (pos + j >= s_len) return false;
        if (s[pos + j] != delim[j]) return false;
        return cstr_find_delim(s, s_len, delim, delim_len, pos, j + 1);
}

AOCLIBS_PREFIX size_t str_chr_cstr(const str *xref s, const char *xref delim,
                                   const size_t delim_len) {
        ASSERT_NONNULL(str_is_null_assert(s));
        ASSERT_NONNULL(delim != NULL);

        for (size_t i = 0; i < s->len; i++) {
                if (cstr_find_delim(s->str, s->len, delim, delim_len, i, 0)) {
                        return (size_t)i;
                }
        }

        return StrError;
}

AOCLIBS_PREFIX size_t str_chr(const str *ref s, char delim) {
        ASSERT_NONNULL(str_is_null_assert(s));

        for (size_t i = 0; i < s->len; i++) {
                if (s->str[i] == delim) return i;
        }

        return StrError;
}

AOCLIBS_PREFIX const char *null str_tok_cstr(const str *xref s, const char *xref delim,
                                             const size_t delim_len) {
        ASSERT_NONNULL(str_is_null_assert(s));
        ASSERT_NONNULL(delim != NULL);

        size_t pos = str_chr_cstr(s, delim, delim_len);
        if (pos == StrError) return NULL;

        return s->str + pos + delim_len;
}

AOCLIBS_PREFIX const char *null str_tok(const str *ref s, char delim) {
        ASSERT_NONNULL(str_is_null_assert(s));

        size_t pos = str_chr(s, delim);
        if (pos == StrError) return NULL;

        return s->str + pos + 1;
}

AOCLIBS_PREFIX size_t cstr_trim_whitespace(char *ref s, const size_t len) {
        ASSERT_NONNULL(s != NULL);

        if (len == 0) return 0;

        size_t i = len;
        while (i > 0 && isspace((unsigned char)s[i - 1])) {
                i--;
        }
        s[i] = '\0';
        return len;
}

AOCLIBS_PREFIX void str_trim_whitespace(str *ref s) {
        ASSERT_NONNULL(str_is_null_assert(s));
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");

        size_t len = cstr_trim_whitespace(s->str, s->len);
        s->len = len;
}

AOCLIBS_PREFIX double cstr_to_double(const char *ref s, const double _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        double val = strtod(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

AOCLIBS_PREFIX bool cstr_to_bool(const char *ref s, const bool _default) {
        ASSERT_NONNULL(s != NULL);
        if (cstr_eq_case(s, "true") || cstr_eq(s, "1")) return true;
        if (cstr_eq_case(s, "false") || cstr_eq(s, "0")) return false;
        return _default;
}

AOCLIBS_PREFIX float cstr_to_float(const char *ref s, const float _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        float val = strtof(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

AOCLIBS_PREFIX long cstr_to_long(const char *ref s, const long _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        long val = strtol(s, &endptr, 10);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

#endif // AOCLIBS_STRING_H_
