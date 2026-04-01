#pragma once

#include "cstr.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

fn char *cstr_dup(const char *s, const size_t len) {
    ASSERT_NONNULL(s != NULL);
    char *d = malloc(len);
    if (!d) return NULL;
    return memcpy(d, s, len);
}

fn void cstr_to_lower(char *s) {
    ASSERT_NONNULL(s != NULL);
    for (; *s; s++)
        *s = tolower(*s);
}

fn void cstrn_to_lower(char *s, const size_t len) {
    ASSERT_NONNULL(s != NULL);
    for (size_t i = 0; i < len; i++)
        s[i] = tolower(s[i]);
}

fn bool cstr_ends_with(const char *s, const size_t s_len, const char *pattern, size_t pattern_len) {
    ASSERT_NONNULL(s);
    ASSERT_NONNULL(pattern);
    if (s_len < pattern_len) return false;
    return memcmp(s + s_len - pattern_len, pattern, pattern_len) == 0;
}

fn bool
cstr_begins_with(const char *s, const size_t s_len, const char *pattern, size_t pattern_len) {
    return cstrn_eq(s, s_len, pattern, pattern_len);
}

fn bool cstrn_eq(const char *s, const size_t s_len, const char *pattern, size_t pattern_len) {
    if (!s || !pattern || pattern_len == 0 || pattern_len > s_len) return false;
    if (s_len < pattern_len) return false;
    return memcmp(s, pattern, pattern_len) == 0;
}

fn bool cstrn_eq_case(const char *s, const size_t s_len, const char *pattern, size_t pattern_len) {
    if (!s || !pattern || pattern_len == 0 || pattern_len > s_len) return false;

    char *s_tmp = cstr_dup(s, s_len);
    if (s_tmp == NULL) {
        errno = ENOMEM;
        return false;
    }

    cstrn_to_lower(s_tmp, s_len);

    char *pattern_tmp = cstr_dup(pattern, pattern_len);
    if (pattern_tmp == NULL) {
        errno = ENOMEM;
        return false;
    }

    cstrn_to_lower(pattern_tmp, pattern_len);

    bool equal = memcmp(s_tmp, pattern_tmp, pattern_len) == 0;

    free(s_tmp);
    free(pattern_tmp);

    return equal;
}

fn bool cstr_has(const char *s, size_t s_len, const char *pattern, size_t pattern_len) {
    if (!s || !pattern || pattern_len == 0 || pattern_len > s_len) return false;

    const char *s_ptr = s;
    size_t remaining_len = s_len;

    if (pattern_len == 1) return memchr(s_ptr, *pattern, s_len) != NULL;

    while ((s_ptr = (const char *)memchr(s_ptr, pattern[0], remaining_len)) != NULL) {
        remaining_len = s_len - (s_ptr - s);
        if (remaining_len >= pattern_len && memcmp(s_ptr, pattern, pattern_len) == 0) return true;
        s_ptr++;
        remaining_len--;
    }

    return false;
}

fn size_t cstr_has_at(const char *s, size_t s_len, const char *pattern, size_t pattern_len) {
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

size_t index_of(const char *s, char delim, size_t size) {
    ASSERT_NONNULL(s != NULL);

    const char *ptr = memchr(s, delim, size);
    if (ptr == NULL) return SIZE_MAX;

    return ptr - s;
}

fn int cstr_fmt_size(const char *fmt, ...) {
    ASSERT_NONNULL(fmt != NULL);

    va_list args;
    va_start(args, fmt);
    int needed_len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    return needed_len;
}

fn int cstr_fmt_write(char *s, const size_t s_cap, const char *fmt, ...) {
    ASSERT_NONNULL(s != NULL);
    ASSERT_NONNULL(fmt != NULL);

    int allocated_len = 0;

    va_list args;
    va_start(args, fmt);
    allocated_len = vsnprintf(s, s_cap, fmt, args);
    va_end(args);

    return allocated_len;
}

fn double cstr_to_double(const char *s, const double _default) {
    ASSERT_NONNULL(s != NULL);
    char *endptr;
    double val = strtod(s, &endptr);
    if (*endptr != '\0') {
        return _default;
    }
    return val;
}

fn bool cstr_to_bool(const char *s, const bool _default) {
    ASSERT_NONNULL(s != NULL);
    if (cstr_eq_case(s, "true") || cstr_eq(s, "1")) return true;
    if (cstr_eq_case(s, "false") || cstr_eq(s, "0")) return false;
    return _default;
}

fn float cstr_to_float(const char *s, const float _default) {
    ASSERT_NONNULL(s != NULL);
    char *endptr;
    float val = strtof(s, &endptr);
    if (*endptr != '\0') {
        return _default;
    }
    return val;
}

fn long cstr_to_long(const char *s, const long _default) {
    ASSERT_NONNULL(s != NULL);
    char *endptr;
    long val = strtol(s, &endptr, 10);
    if (*endptr != '\0') {
        return _default;
    }
    return val;
}
