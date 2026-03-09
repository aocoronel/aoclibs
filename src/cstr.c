#pragma once

#include "cstr.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1 / UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX / 2 + 1))
#define HASZERO(x) (((x) - ONES) & ~(x) & HIGHS)

CSlice aoc_extract_between(const char *s, size_t begin, size_t end, char delim) {

        const char *s_ptr = s + begin;
        size_t n_begin = begin;
        size_t n_end = end;

        while (s_ptr < s + end && *s_ptr == ' ') {
                s_ptr++;
                n_begin++;
        }

        s_ptr = s + end - 1;
        while (s_ptr >= s + n_begin && *s_ptr == ' ') {
                s_ptr--;
                n_end--;
        }

        return (CSlice){ .data = s + n_begin, .len = n_end - n_begin };
}

void cslice_to_cstr(CSlice s, char *buff, const size_t size) {
        int size_to_copy = s.len > size ? size : s.len;
        memcpy(buff, s.data, size_to_copy);
        buff[size_to_copy] = '\0';
}

AOCDEF char *aoc_cstr_dup(const char *s, const size_t len) {
        ASSERT_NONNULL(s != NULL);
        char *d = CSTR_MALLOC(len);
        if (!d) return NULL;
        return memcpy(d, s, len);
}

AOCDEF void aoc_cstr_to_lower(char *s) {
        ASSERT_NONNULL(s != NULL);
        for (; *s; s++)
                *s = tolower(*s);
}

AOCDEF void aoc_cstrn_to_lower(char *s, const size_t len) {
        ASSERT_NONNULL(s != NULL);
        for (size_t i = 0; i < len; i++)
                s[i] = tolower(s[i]);
}

AOCDEF bool
aoc_cstr_ends_with(const char *s, const size_t s_len, const char *pattern, size_t pattern_len) {
        ASSERT_NONNULL(s);
        ASSERT_NONNULL(pattern);
        if (s_len < pattern_len) return false;
        return memcmp(s + s_len - pattern_len, pattern, pattern_len) == 0;
}

AOCDEF bool
aoc_cstr_begins_with(const char *s, const size_t s_len, const char *pattern, size_t pattern_len) {
        return aoc_cstrn_eq(s, s_len, pattern, pattern_len);
}

AOCDEF bool
aoc_cstrn_eq(const char *s, const size_t s_len, const char *pattern, size_t pattern_len) {
        if (!s || !pattern || pattern_len == 0 || pattern_len > s_len) return false;
        if (s_len < pattern_len) return false;
        return memcmp(s, pattern, pattern_len) == 0;
}

AOCDEF bool
aoc_cstrn_eq_case(const char *s, const size_t s_len, const char *pattern, size_t pattern_len) {
        if (!s || !pattern || pattern_len == 0 || pattern_len > s_len) return false;

        char *s_tmp = aoc_cstr_dup(s, s_len);
        if (s_tmp == NULL) {
                errno = ENOMEM;
                return false;
        }

        aoc_cstrn_to_lower(s_tmp, s_len);

        char *pattern_tmp = aoc_cstr_dup(pattern, pattern_len);
        if (pattern_tmp == NULL) {
                errno = ENOMEM;
                return false;
        }

        aoc_cstrn_to_lower(pattern_tmp, pattern_len);

        bool equal = memcmp(s_tmp, pattern_tmp, pattern_len) == 0;

        free(s_tmp);
        free(pattern_tmp);

        return equal;
}

AOCDEF bool
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

AOCDEF size_t aoc_cstr_has_at(const char *s,
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

size_t aoc_index_of(const char *s, char delim, size_t size) {
        ASSERT_NONNULL(s != NULL);

        const char *ptr = memchr(s, delim, size);
        if (ptr == NULL) return SIZE_MAX;

        return ptr - s;
}

AOCDEF size_t aoc_cstrcpy_size(size_t dest_size,
                                       const size_t dest_offset,
                                       const size_t src_len) {
        size_t needed_size = src_len + 1;
        return needed_size > dest_size - dest_offset ? needed_size : dest_size;
}

#ifdef AOCLIBS_CSTRCPY_AS_MEMCPY
AOCDEF size_t aoc_cstrcpy(char *dest, const char *src, size_t dest_cap) {
        const char *ptr = memcpy(dest, src, dest_cap);
        return ptr - dest;
}
#else
AOCDEF size_t aoc_cstrcpy(char *dest, const char *src, size_t dest_cap) {
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
#endif

AOCDEF
size_t aoc_cstrappend(char *dest, const char *src, size_t dest_len, size_t dest_cap) {
        size_t len = aoc_cstrcpy(dest + dest_len + 1, src, dest_cap);
        dest[dest_len] = ' ';
        return len;
}

AOCDEF size_t aoc_cstrcat(char *dest, const char *src, size_t dest_len, size_t dest_cap) {
        return aoc_cstrcpy(dest + dest_len, src, dest_cap);
}

AOCDEF int aoc_cstr_fmt_size(const char *fmt, ...) {
        ASSERT_NONNULL(fmt != NULL);

        va_list args;
        va_start(args, fmt);
        int needed_len = vsnprintf(NULL, 0, fmt, args);
        va_end(args);

        return needed_len;
}

AOCDEF int aoc_cstr_fmt_write(char *s, const size_t s_cap, const char *fmt, ...) {
        ASSERT_NONNULL(s != NULL);
        ASSERT_NONNULL(fmt != NULL);

        int allocated_len = 0;

        va_list args;
        va_start(args, fmt);
        allocated_len = vsnprintf(s, s_cap, fmt, args);
        va_end(args);

        return allocated_len;
}

AOCDEF double aoc_cstr_to_double(const char *s, const double _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        double val = strtod(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

AOCDEF bool aoc_cstr_to_bool(const char *s, const bool _default) {
        ASSERT_NONNULL(s != NULL);
        if (aoc_cstr_eq_case(s, "true") || aoc_cstr_eq(s, "1")) return true;
        if (aoc_cstr_eq_case(s, "false") || aoc_cstr_eq(s, "0")) return false;
        return _default;
}

AOCDEF float aoc_cstr_to_float(const char *s, const float _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        float val = strtof(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

AOCDEF long aoc_cstr_to_long(const char *s, const long _default) {
        ASSERT_NONNULL(s != NULL);
        char *endptr;
        long val = strtol(s, &endptr, 10);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}
