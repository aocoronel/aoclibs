#pragma once

#include "cstr.h"
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

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

AOCLIBS_PREFIX bool aoc_cstr_ends_with(const char *xref s,
                                       const size_t s_len,
                                       const char *xref pattern,
                                       size_t pattern_len) {
        ASSERT_NONNULL(s);
        ASSERT_NONNULL(pattern);
        if (s_len < pattern_len) return false;
        return memcmp(s + s_len - pattern_len, pattern, pattern_len) == 0;
}

AOCLIBS_PREFIX bool aoc_cstr_begins_with(const char *xref s,
                                         const size_t s_len,
                                         const char *xref pattern,
                                         size_t pattern_len) {
        return aoc_cstrn_eq(s, s_len, pattern, pattern_len);
}

AOCLIBS_PREFIX bool
aoc_cstrn_eq(const char *xref s, const size_t s_len, const char *xref pattern, size_t pattern_len) {
        ASSERT_NONNULL(s);
        ASSERT_NONNULL(pattern);
        if (s_len < pattern_len) return false;
        return memcmp(s, pattern, pattern_len) == 0;
}

AOCLIBS_PREFIX bool aoc_cstrn_eq_case(const char *xref s,
                                      const size_t s_len,
                                      const char *xref pattern,
                                      size_t pattern_len) {
        ASSERT_NONNULL(s);
        ASSERT_NONNULL(pattern);
        if (s_len < pattern_len) return false;

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

AOCLIBS_PREFIX size_t aoc_cstrcpy_size(size_t dest_buff,
                                         const size_t dest_offset,
                                         const size_t src_len) {
        size_t needed_size = src_len + 1;
        return needed_size > dest_buff - dest_offset ? needed_size : dest_buff;
}

AOCLIBS_PREFIX size_t aoc_cstrcpy(char *xref dest, const char *xref src, size_t dest_cap) {
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

AOCLIBS_PREFIX int aoc_cstrcpy_fmt_size(const char *fmt, ...) {
        ASSERT_NONNULL(fmt != NULL);

        va_list args;
        va_start(args, fmt);
        int needed_len = vsnprintf(NULL, 0, fmt, args);
        va_end(args);

        return needed_len;
}

AOCLIBS_PREFIX int aoc_cstrcpy_fmt(char *xref s, const size_t s_cap, const char *xref fmt, ...) {
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
