#pragma once

#include "cstr.h"
#include "base.h"
#include <ctype.h>

char *cstr_dup(const char *s, const size_t len) {
    ASSERT_NONNULL(s != NULL);

    char *d = (char *)malloc(len);
    if (!d) return NULL;

    memcpy(d, s, len);
    return d;
}

void cstr_to_lower(char *s) {
    ASSERT_NONNULL(s != NULL);
    for (; *s; s++)
        *s = tolower(*s);
}

void cstrn_to_lower(char *s, const size_t len) {
    ASSERT_NONNULL(s != NULL);
    range(0, len, i) s[i] = tolower(s[i]);
}

void cstr_to_upper(char *s) {
    ASSERT_NONNULL(s != NULL);
    for (; *s; s++)
        *s = toupper(*s);
}

void cstrn_to_upper(char *s, const size_t len) {
    ASSERT_NONNULL(s != NULL);
    range(0, len, i) s[i] = toupper(s[i]);
}

bool cstr_ends_with(const char *s,
                    const size_t s_len,
                    const char *pattern,
                    const size_t pattern_len) {
    ASSERT_NONNULL(s);
    ASSERT_NONNULL(pattern);
    if (s_len < pattern_len) return false;
    return memcmp(s + s_len - pattern_len, pattern, pattern_len) == 0;
}

bool cstr_begins_with(const char *s,
                      const size_t s_len,
                      const char *pattern,
                      const size_t pattern_len) {
    ASSERT_NONNULL(s);
    ASSERT_NONNULL(pattern);
    return cstrn_eq(s, s_len, pattern, pattern_len);
}

bool cstrn_eq(const char *s, const size_t s_len, const char *pattern, const size_t pattern_len) {
    ASSERT_NONNULL(s);
    ASSERT_NONNULL(pattern);
    if (pattern_len == 0 || pattern_len > s_len) return false;
    if (s_len < pattern_len) return false;
    return memcmp(s, pattern, pattern_len) == 0;
}

char cstrn_eq_case(const char *s,
                   const size_t s_len,
                   const char *pattern,
                   const size_t pattern_len) {
    ASSERT_NONNULL(s);
    ASSERT_NONNULL(pattern);
    if (pattern_len == 0 || pattern_len > s_len) return false;

    char *ptr = (char *)malloc(sizeof(char) * s_len + pattern_len);
    if (!ptr) return -1;

    char *s_tmp = ptr;
    char *pattern_tmp = ptr + s_len;

    memcpy(s_tmp, s, s_len);
    memcpy(pattern_tmp, pattern, pattern_len);

    cstrn_to_lower(s_tmp, s_len);
    cstrn_to_lower(pattern_tmp, pattern_len);

    bool equal = memcmp(s_tmp, pattern_tmp, pattern_len) == 0;

    free(ptr);

    return equal;
}

bool cstr_has(const char *s, const size_t s_len, const char *pattern, const size_t pattern_len) {
    ASSERT_NONNULL(s);
    ASSERT_NONNULL(pattern);
    if (pattern_len == 0 || pattern_len > s_len) return false;

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

size_t
cstr_has_at(const char *s, const size_t s_len, const char *pattern, const size_t pattern_len) {
    ASSERT_NONNULL(s);
    ASSERT_NONNULL(pattern);
    if (pattern_len == 0 || pattern_len > s_len) return false;

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

    const char *ptr = (const char *)memchr(s, delim, size);
    if (ptr == NULL) return SIZE_MAX;

    return ptr - s;
}

int cstr_fmt_size(const char *fmt, ...) {
    ASSERT_NONNULL(fmt != NULL);

    va_list args;
    va_start(args, fmt);
    int needed_len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    return needed_len;
}

int cstr_fmt_write(char *s, const size_t s_cap, const char *fmt, ...) {
    ASSERT_NONNULL(s != NULL);
    ASSERT_NONNULL(fmt != NULL);

    int allocated_len = 0;

    va_list args;
    va_start(args, fmt);
    allocated_len = vsnprintf(s, s_cap, fmt, args);
    va_end(args);

    return allocated_len;
}

double cstr_to_double(const char *s, const double _default) {
    ASSERT_NONNULL(s != NULL);
    char *endptr;
    double val = strtod(s, &endptr);
    if (*endptr != '\0') {
        return _default;
    }
    return val;
}

bool cstr_to_bool(const char *s, const bool _default) {
    ASSERT_NONNULL(s != NULL);
    if (cstr_eq_case(s, "true") || cstr_eq(s, "1")) return true;
    if (cstr_eq_case(s, "false") || cstr_eq(s, "0")) return false;
    return _default;
}

float cstr_to_float(const char *s, const float _default) {
    ASSERT_NONNULL(s != NULL);
    char *endptr;
    float val = strtof(s, &endptr);
    if (*endptr != '\0') {
        return _default;
    }
    return val;
}

long cstr_to_long(const char *s, const long _default) {
    ASSERT_NONNULL(s != NULL);
    char *endptr;
    long val = strtol(s, &endptr, 10);
    if (*endptr != '\0') {
        return _default;
    }
    return val;
}

Slice while_next_word(const char *restrict s, size_t *restrict begin, size_t end) {
    ASSERT_NONNULL(s);
    size_t i = *begin;

    while (i < end && isspace((unsigned char)s[i])) {
        i++;
    }

    size_t start = i;

    while (i < end && !isspace((unsigned char)s[i])) {
        i++;
    }

    *begin = i;

    return (Slice){ .data = s + start, .len = i - start };
}

Slice while_next_word_and(const char *restrict s, size_t *restrict begin, size_t end, char delim) {
    ASSERT_NONNULL(s);

    size_t i = *begin;

    while (i < end && isspace((unsigned char)s[i])) {
        i++;
    }

    size_t start = i;

    bool found_delim = false;

    while (i < end && !isspace((unsigned char)s[i])) {
        if (s[i] == delim) {
            found_delim = true;
            break;
        }
        i++;
    }

    *begin = i;

    if (found_delim) {
        return (Slice){ .data = s + start, .len = i - start };
    } else {
        return (Slice){ .data = s + start, .len = i - start };
    }
}

void slice_to_cstr(Slice s, char *buff, const size_t size) {
    int size_to_copy = s.len > size ? size : s.len;
    memcpy(buff, s.data, size_to_copy);
    buff[size_to_copy] = '\0';
}

size_t cstr_skip_whitespace_forward(const char *pos, size_t len) {
    ASSERT_NONNULL(pos);
    size_t i = 0;
    while (i < len && isspace((unsigned char)pos[i]))
        i++;
    return i;
}

size_t cstr_skip_whitespace_backward(const char *pos, size_t len) {
    ASSERT_NONNULL(pos);
    size_t i = 0;
    while (i < len && isspace((unsigned char)pos[len - 1 - i]))
        i++;
    return i;
}

void slice_skip_whitespace_backward(Slice *s) {
    size_t skipped_whitesapce = cstr_skip_whitespace_backward(s->data, s->len);
    s->len -= skipped_whitesapce;
}

void slice_skip_whitespace_forward(Slice *s) {
    size_t skipped_whitesapce = cstr_skip_whitespace_forward(s->data, s->len);
    s->data += skipped_whitesapce;
    s->len -= skipped_whitesapce;
}

void slice_trim(Slice *s) {
    slice_skip_whitespace_forward(s);
    slice_skip_whitespace_backward(s);
}

void slice_chop_right_by(Slice *s, char delim) {
    ASSERT_NONNULL(s);
    const char *ptr = (const char *)memchr(s->data, delim, s->len);
    if (!ptr) return;

    s->len = (size_t)(ptr - s->data);
}

Slice slice_extract_from_substring(Slice *s) {
    ASSERT_NONNULL(s);
    if (s->len == 0 || s->data[0] != '"') {
        return *s;
    }

    const char *pos = s->data + 1;
    size_t len = 0;

    while (len < s->len - 1) {
        if (pos[len] == '\\') {
            if (len + 1 < s->len - 1)
                len += 2;
            else
                break;
        } else if (pos[len] == '"') {
            break;
        } else {
            len++;
        }
    }

    return (Slice){
        .data = pos,
        .len = len,
    };
}

void slice_shift_by(Slice *s, size_t len) {
    s->data += len;
    s->len -= len;
}
