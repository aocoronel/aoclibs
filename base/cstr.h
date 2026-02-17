#ifndef AOCLIBS_CSTR_H_
#define AOCLIBS_CSTR_H_

#define AOCLIBS_IMPLEMENTATION
#include "base.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// --> printf("%s "SLICE_FMT" stuff...", "do", VSLICE(myslice));
#define SLICE_FMT "%.*s"
#define VSLICE(r) ((r).len), ((r).data)

typedef struct {
        size_t len;
        const char *data;
} cslice;

#ifndef CSTR_MALLOC
        #define CSTR_MALLOC malloc
#endif

#ifdef AOCLIBS_STRIP_PREFIX
        #define lcslice aoc_lcslice
        #define cstr_to_slice aoc_cstr_to_slice

        #define cstrdup aoc_cstr_dup
        #define cstr_to_lower aoc_cstr_to_lower
        #define cstrn_to_lower aoc_cstrn_to_lower

        #define cslice_eq aoc_cslice_eq
        #define cslice_begins_with aoc_cslice_begins_with
        #define cslice_ends_with aoc_cslice_ends_with
        #define cstr_begins_with aoc_cstr_begins_with
        #define cstr_ends_with aoc_cstr_ends_with
        #define cstr_eq aoc_cstr_eq
        #define cstrn_eq aoc_cstrn_eq
        #define cstr_eq_case aoc_cstr_eq_case
        #define cstrn_eq_case aoc_cstrn_eq_case

        #define cstrhas aoc_cstr_has

        #define cstrcopy_size aoc_cstr_copy_size

        #define cstrcopy aoc_cstr_copy
        #define cstrcat aoc_cstrcat
        #define cstrappend aoc_cstrappend

        #ifndef AOCLIBS_NO_STDIO
                #define cstrcopy_fmt_size aoc_cstr_copy_fmt_size
                #define cstrcopy_fmt aoc_cstr_copy_fmt
        #endif

        #define cstrstr_index aoc_cstrstr_index
        #define cstrnstr_index aoc_cstrnstr_index

        #define cstrstr_tok aoc_cstrstr_tok
        #define cstrtok aoc_cstrtok
        #define lcstrtok aoc_lcstrtok
        #define cstrstrtok aoc_cstr_tok

        #define cstr_trim_whitespace aoc_cstr_trim_whitespace

        #define cstr_to_bool aoc_cstr_to_bool
        #define cstr_to_double aoc_cstr_to_double
        #define cstr_to_float aoc_cstr_to_float
        #define cstr_to_long aoc_cstr_to_long

        #define cstrlen aoc_cstrlen

        #define index_of aoc_index_of
        #define index_of_till aoc_index_of_till
#endif

#define aoc_cstr_to_slice(s, start, end)                      \
        (cslice) {                                            \
                .data = (s) + (start), .len = (end) - (start) \
        }

// cslice myslice = lcslice("hello, world!");
#define aoc_lcslice(s)                                               \
        (cslice) {                                                   \
                .data = "" s "", .len = STRLEN(s), .cap = STRLEN(s), \
        }

AOCLIBS_PREFIX char *xnull aoc_cstr_dup(const char *ref s, const size_t len);
AOCLIBS_PREFIX void aoc_cstr_to_lower(char *ref s);
AOCLIBS_PREFIX void aoc_cstrn_to_lower(char *ref s, const size_t len);

/*
 * Compares s and pattern. Assume strings are null-terminated.
*/
AOCLIBS_PREFIX bool aoc_cstr_eq(const char *xref s, const char *xref pattern);

#define aoc_cslice_eq(cs1, cs2) aoc_cstrn_eq((cs1)->data, (cs1)->len, (cs2)->data, (cs2)->len)

#define aoc_cslice_begins_with(cs, begin, begin_len) \
        aoc_cstrn_eq((cs)->data, (cs)->len, (begin), (begin_len))
#define aoc_cslice_ends_with(cs, end, end_len) \
        aoc_cstrn_eq((cs)->data + (cs)->len, (cs)->len, (end), (end_len))

#define aoc_cstr_begins_with(s, s_len, begin, begin_len) \
        aoc_cstrn_eq((s), (s_len), (begin), (begin_len))
#define aoc_cstr_ends_with(s, s_len, end, end_len) \
        aoc_cstrn_eq((s) + (s_len), (s_len), (end), (end_len))
/*
 * Compares s to pattern.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Returns if s is equal to pattern.
*/
AOCLIBS_PREFIX bool
aoc_cstrn_eq(const char *xref s, const size_t s_len, const char *xref pattern, size_t pattern_len);

AOCLIBS_PREFIX bool aoc_cstr_eq_case(const char *xref s, const char *xref pattern);

#define aoc_cslice_eq_case(cs1, cs2, offset) \
        aoc_cstrn_eq_case((cs1)->data + (offset), (cs1)->len, (cs2)->data, (cs2)->len)
AOCLIBS_PREFIX bool aoc_cstrn_eq_case(const char *xref s,
                                      const size_t s_len,
                                      const char *xref pattern,
                                      size_t pattern_len);

AOCLIBS_PREFIX bool
aoc_cstr_has(const char *xref s, size_t s_len, const char *xref pattern, size_t pattern_len);

/*
 * Returns needed dest size to fit src.
 *
 * This is particularly useful, when copying the contents of src to dest, to ensure the string is
 * not going to be truncated.
*/
AOCLIBS_PREFIX size_t aoc_cstr_copy_size(size_t dest_buff,
                                         const size_t dest_offset,
                                         const size_t src_len);

#define aoc_cstrcat(dest, src, dest_len, dest_cap) \
        aoc_cstr_copy((dest) + (dest_len), (src), (dest_cap))
#define aoc_cstrappend(dest, src, dest_len, dest_cap) \
        aoc_cstr_copy((dest) + (dest_len + 1), (src), (dest_cap))
/*
 * Copies src to dest, until dest_cap is reached. This function guarantees the
 * dest will be null-terminated.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Returns new length of dest
 *
 * This is a modified version of __stpncpy from the Musl LibC
*/
AOCLIBS_PREFIX size_t aoc_cstr_copy(char *xref dest, const char *xref src, size_t dest_cap);

#ifndef AOCLIBS_NO_STDIO
        #include <stdarg.h>
/*
 * Returns needed dest size to fit src.
 *
 * This is obligatory to prevent string truncation when using aoc_cstr_copy_fmt.
*/
AOCLIBS_PREFIX int aoc_cstr_copy_fmt_size(const char *xref fmt, ...);
AOCLIBS_PREFIX int aoc_cstr_copy_fmt(char *xref s, const size_t s_cap, const char *xref fmt, ...);
#endif

/*
 * Finds pattern in s. Assume strings are null-terminated.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Error:
 * - SIZE_MAX :: Pattern not found
 *
 * Returns the index where pattern starts in s.
*/
AOCLIBS_PREFIX size_t aoc_cstrstr_index(const char *xref s, const char *xref pattern);

AOCLIBS_PREFIX size_t aoc_cstrnstr_index(const char *xref s,
                                         const char *xref pattern,
                                         const size_t s_len,
                                         const size_t pattern_len);

#define aoc_cstrtok(s, delim, s_len) aoc_cstr_tok((s), (#delim), 1)
#define aoc_lcstrtok(s, delim) aoc_cstr_tok((s), (#delim), STRLEN(s), 1)
#define aoc_cstrstrtok aoc_cstr_tok
AOCLIBS_PREFIX const char *null aoc_cstrstr_tok(const char *ref s,
                                                const char *delim,
                                                const size_t s_len,
                                                const size_t delim_len);

AOCLIBS_PREFIX size_t aoc_cstr_trim_whitespace(char *ref s, const size_t len);

AOCLIBS_PREFIX double aoc_cstr_to_double(const char *ref s, const double _default);
AOCLIBS_PREFIX bool aoc_cstr_to_bool(const char *ref s, const bool _default);
AOCLIBS_PREFIX float aoc_cstr_to_float(const char *ref s, const float _default);
AOCLIBS_PREFIX long aoc_cstr_to_long(const char *ref s, const long _default);

size_t aoc_index_of(const char *ref buff, char delim);
size_t aoc_index_of_till(const char *ref buff, char delim, size_t size);

#ifdef AOCLIBS_IMPLEMENTATION

        // clang-format off
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
// clang-format on

void cslice_to_cstr(cslice s, char *buff, const size_t size) {
        int size_to_copy = s.len > size ? size : s.len;
        memcpy(buff, s.data, size_to_copy);
        buff[size_to_copy] = '\0';
}

AOCLIBS_PREFIX char *xnull aoc_cstr_dup(const char *ref s, const size_t len) {
        ASSERT_NONNULL(s != NULL);
        char *d = CSTR_MALLOC(len);
        if (!d) return NULL;
        return memcpy(d, s, len);
}

AOCLIBS_PREFIX void aoc_cstr_to_lower(char *ref s) {
        ASSERT_NONNULL(s != NULL);
        for (; *s; s++)
                *s = tolower(*s);
}

AOCLIBS_PREFIX void aoc_cstrn_to_lower(char *ref s, const size_t len) {
        ASSERT_NONNULL(s != NULL);
        for (size_t i = 0; i < len; i++)
                s[i] = tolower(s[i]);
}

AOCLIBS_PREFIX bool aoc_cstr_eq(const char *xref s, const char *xref pattern) {
        size_t s_len = strlen(s);
        size_t pattern_len = strlen(pattern);
        if (!s || !pattern || pattern_len > s_len) return false;

        const char *s_ptr = s;
        const char *p_ptr = pattern;

        if (((size_t)s_ptr & (ALIGN - 1)) == ((size_t)p_ptr & (ALIGN - 1))) {
                size_t *ws = (size_t *)s_ptr;
                size_t *wp = (size_t *)p_ptr;

                while (((size_t)s_ptr & (ALIGN - 1)) && pattern_len--) {
                        if (*s_ptr++ != *p_ptr++) return false;
                }

                size_t word_len = pattern_len / sizeof(size_t);
                size_t i;
                for (i = 0; i < word_len; i++) {
                        if (ws[i] != wp[i]) return false;
                }

                s_ptr = (const char *)(ws + i);
                p_ptr = (const char *)(wp + i);
                for (i = 0; i < (pattern_len % sizeof(size_t)); i++) {
                        if (s_ptr[i] != p_ptr[i]) return false;
                }

                return true;
        } else {
                for (size_t i = 0; i < pattern_len; i++) {
                        if (s_ptr[i] != p_ptr[i]) return false;
                }
                return true;
        }
}

AOCLIBS_PREFIX bool
aoc_cstrn_eq(const char *xref s, const size_t s_len, const char *xref pattern, size_t pattern_len) {
        if (!s || !pattern || pattern_len > s_len) return false;

        const char *s_ptr = s;
        const char *p_ptr = pattern;

        if (((size_t)s_ptr & (ALIGN - 1)) == ((size_t)p_ptr & (ALIGN - 1))) {
                size_t *ws = (size_t *)(s_ptr);
                size_t *wp = (size_t *)(p_ptr);

                while (((size_t)s_ptr & (ALIGN - 1)) && pattern_len--) {
                        if (*s_ptr++ != *p_ptr++) return false;
                }

                size_t word_len = pattern_len / sizeof(size_t);
                size_t i;
                for (i = 0; i < word_len; i++) {
                        if (ws[i] != wp[i]) return false;
                }

                s_ptr = (const char *)(ws + i);
                p_ptr = (const char *)(wp + i);
                for (i = 0; i < (pattern_len % sizeof(size_t)); i++) {
                        if (s_ptr[i] != p_ptr[i]) return false;
                }

                return true;
        } else {
                for (size_t i = 0; i < pattern_len; i++) {
                        if (s_ptr[i] != p_ptr[i]) return false;
                }
                return true;
        }
}

AOCLIBS_PREFIX bool aoc_cstrn_eq_case(const char *xref s,
                                      const size_t s_len,
                                      const char *xref pattern,
                                      size_t pattern_len) {
        if (!s || !pattern || pattern_len > s_len) return false;

        const char *s_ptr = s;
        const char *p_ptr = pattern;

        if (((size_t)s_ptr & (ALIGN - 1)) == ((size_t)p_ptr & (ALIGN - 1))) {
                size_t *ws = (size_t *)(s_ptr);
                size_t *wp = (size_t *)(p_ptr);

                while (((size_t)s_ptr & (ALIGN - 1)) && pattern_len--) {
                        if (tolower(*s_ptr++) != tolower(*p_ptr++)) return false;
                }

                size_t word_len = pattern_len / sizeof(size_t);
                size_t i;
                for (i = 0; i < word_len; i++) {
                        if (tolower(ws[i]) != tolower(wp[i])) return false;
                }

                s_ptr = (const char *)(ws + i);
                p_ptr = (const char *)(wp + i);
                for (i = 0; i < (pattern_len % sizeof(size_t)); i++) {
                        if (tolower(s_ptr[i]) != tolower(p_ptr[i])) return false;
                }

                return true;
        } else {
                for (size_t i = 0; i < pattern_len; i++) {
                        if (tolower(s_ptr[i]) != tolower(p_ptr[i])) return false;
                }
                return true;
        }
}

AOCLIBS_PREFIX bool aoc_cstr_eq_case(const char *xref s, const char *xref pattern) {
        size_t s_len = strlen(s);
        size_t pattern_len = strlen(pattern);
        if (!s || !pattern || pattern_len > s_len) return false;

        const char *s_ptr = s;
        const char *p_ptr = pattern;

        if (((size_t)s_ptr & (ALIGN - 1)) == ((size_t)p_ptr & (ALIGN - 1))) {
                size_t *ws = (size_t *)s_ptr;
                size_t *wp = (size_t *)p_ptr;

                while (((size_t)s_ptr & (ALIGN - 1)) && pattern_len--) {
                        if (tolower(*s_ptr++) != tolower(*p_ptr++)) return false;
                }

                size_t word_len = pattern_len / sizeof(size_t);
                size_t i;
                for (i = 0; i < word_len; i++) {
                        if (tolower(ws[i]) != tolower(wp[i])) return false;
                }

                s_ptr = (const char *)(ws + i);
                p_ptr = (const char *)(wp + i);
                for (i = 0; i < (pattern_len % sizeof(size_t)); i++) {
                        if (tolower(s_ptr[i]) != tolower(p_ptr[i])) return false;
                }

                return true;
        } else {
                for (size_t i = 0; i < pattern_len; i++) {
                        if (tolower(s_ptr[i]) != tolower(p_ptr[i])) return false;
                }
                return true;
        }
}

AOCLIBS_PREFIX bool
aoc_cstr_has(const char *s, size_t s_len, const char *pattern, size_t pattern_len) {
        if (!s || !pattern || pattern_len == 0 || pattern_len > s_len) return false;

        const char *s_ptr = s;
        size_t remaining_len = s_len;

        if (pattern_len == 1) return memchr(s_ptr, *pattern, s_len) != NULL;

        while ((s_ptr = (const char *)memchr(s_ptr, pattern[0], remaining_len)) != NULL) {
                remaining_len = s_len - (s_ptr - s);
                if (remaining_len >= pattern_len && memcmp(s_ptr, pattern, pattern_len) == 0)
                        return true;
                s_ptr++;
                remaining_len--;
        }

        return false;
}

AOCLIBS_PREFIX size_t aoc_cstr_has_at(const char *s,
                                      size_t s_len,
                                      const char *pattern,
                                      size_t pattern_len) {
        if (!s || !pattern || pattern_len == 0 || pattern_len > s_len) return false;

        const char *s_ptr = s;
        size_t remaining_len = s_len;

        if (pattern_len == 1) {
                s_ptr = (const char *)memchr(s_ptr, *pattern, s_len);
                remaining_len = s_len - (s_ptr - s);
                return s_len - remaining_len;
        }

        while ((s_ptr = (const char *)memchr(s_ptr, pattern[0], remaining_len)) != NULL) {
                remaining_len = s_len - (s_ptr - s);
                if (remaining_len >= pattern_len && memcmp(s_ptr, pattern, pattern_len) == 0)
                        return s_len - remaining_len;
                s_ptr++;
                remaining_len--;
        }

        return SIZE_MAX;
}

AOCLIBS_PREFIX size_t aoc_cstr_copy_size(size_t dest_buff,
                                         const size_t dest_offset,
                                         const size_t src_len) {
        size_t needed_size = src_len + 1;
        return needed_size > dest_buff - dest_offset ? needed_size : dest_buff;
}

AOCLIBS_PREFIX size_t aoc_cstr_copy(char *xref dest, const char *xref src, size_t dest_cap) {
        ASSERT_NONNULL(dest != NULL);
        ASSERT_NONNULL(src != NULL);

        size_t *word_dest;
        const size_t *word_src;
        size_t len = 0;

        if (((size_t)src & ALIGN) == ((size_t)dest & ALIGN)) {
                for (; ((size_t)src & ALIGN) && dest_cap && (*dest = *src);
                     dest_cap--, src++, dest++, len++)
                        ;
                if (!dest_cap || !*src) goto defer;
                word_dest = (void *)dest;
                word_src = (const void *)src;
                for (; dest_cap >= sizeof(size_t) && !HASZERO(*word_src);
                     dest_cap -= sizeof(size_t), word_src++, word_dest++) {
                        *word_dest = *word_src;
                        len += sizeof(size_t);
                }
                dest = (void *)word_dest;
                src = (const void *)word_src;
        }
        for (; dest_cap && (*dest = *src); dest_cap--, src++, dest++, len++)
                ;
defer:
        dest[len] = '\0';
        return len;
}

        #ifndef AOCLIBS_NO_STDIO

AOCLIBS_PREFIX int aoc_cstr_copy_fmt_size(const char *fmt, ...) {
        ASSERT_NONNULL(fmt != NULL);

        va_list args;
        va_start(args, fmt);
        int needed_len = vsnprintf(NULL, 0, fmt, args);
        va_end(args);

        return needed_len;
}

AOCLIBS_PREFIX int aoc_cstr_copy_fmt(char *xref s, const size_t s_cap, const char *xref fmt, ...) {
        ASSERT_NONNULL(s != NULL);
        ASSERT_NONNULL(fmt != NULL);

        int allocated_len = 0;

        va_list args;
        va_start(args, fmt);
        allocated_len = vsnprintf(s, s_cap, fmt, args);
        va_end(args);

        return allocated_len;
}
        #endif

AOCLIBS_PREFIX size_t aoc_cstrstr_index(const char *xref s, const char *xref pattern) {
        ASSERT_NONNULL(s != NULL);
        ASSERT_NONNULL(pattern != NULL);
        if (!*pattern) return SIZE_MAX;

        const char *p = s;
        size_t pattern_len = strlen(pattern);

        while (*p) {
                const char *found = memchr(p, *pattern, strlen(p));
                if (!found) return SIZE_MAX;

                if (strncmp(found, pattern, pattern_len) == 0) return found - s;

                p = found + 1;
        }

        return SIZE_MAX;
}

AOCLIBS_PREFIX size_t aoc_cstrnstr_index(const char *xref s,
                                         const char *xref pattern,
                                         const size_t s_len,
                                         const size_t pattern_len) {
        ASSERT_NONNULL(s != NULL);
        ASSERT_NONNULL(pattern != NULL);
        if (!*pattern) return SIZE_MAX;

        const char *p = s;

        while (*p) {
                const char *found = memchr(p, *pattern, s_len);
                if (!found) return SIZE_MAX;

                if (strncmp(found, pattern, pattern_len) == 0) return found - s;

                p = found + 1;
        }

        return SIZE_MAX;
}

        #define aoc_cstrtok(s, delim, s_len) aoc_cstr_tok((s), (#delim), 1)
        #define aoc_lcstrtok(s, delim) aoc_cstr_tok((s), (#delim), STRLEN(s), 1)
        #define aoc_cstrstrtok aoc_cstr_tok
AOCLIBS_PREFIX const char *null aoc_cstrstr_tok(const char *ref s,
                                                const char *delim,
                                                const size_t s_len,
                                                const size_t delim_len) {
        size_t pos = aoc_cstrnstr_index(s, delim, s_len, delim_len);
        if (pos == SIZE_MAX) return NULL;

        return s + pos + 1;
}

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

AOCLIBS_PREFIX double aoc_cstr_to_double(const char *ref s, const double _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        double val = strtod(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

AOCLIBS_PREFIX bool aoc_cstr_to_bool(const char *ref s, const bool _default) {
        ASSERT_NONNULL(s != NULL);
        if (aoc_cstr_eq_case(s, "true") || aoc_cstr_eq(s, "1")) return true;
        if (aoc_cstr_eq_case(s, "false") || aoc_cstr_eq(s, "0")) return false;
        return _default;
}

AOCLIBS_PREFIX float aoc_cstr_to_float(const char *ref s, const float _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        float val = strtof(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

AOCLIBS_PREFIX long aoc_cstr_to_long(const char *ref s, const long _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        long val = strtol(s, &endptr, 10);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

AOCLIBS_PREFIX size_t aoc_cstrlen(const char *ref s, const size_t size) {
        ASSERT_NONNULL(s != NULL);
        size_t len;
        const char *p = memchr(s, 0, size);
        if (p)
                len = p - s;
        else
                len = size;
        return len;
}

size_t aoc_index_of(const char *ref buff, char delim) {
        ASSERT_NONNULL(buff != NULL);
        size_t i = 0;

        for (; buff && buff[i] != delim && buff[i] != '\0'; i++)
                ;

        if (buff[i] == delim) i++;

        return i == 0 ? -1 : i;
}

size_t aoc_index_of_till(const char *ref buff, char delim, size_t size) {
        ASSERT_NONNULL(buff != NULL);
        size_t i = 0;

        for (; buff && buff[i] != delim && buff[i] != '\0'; i++)
                if (i > size) return -1;

        if (buff[i] == delim) i++;

        return i == 0 ? -1 : i;
}

#endif

#endif // AOCLIBS_CSTR_H_
