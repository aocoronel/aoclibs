#pragma once

#include "base.h"
#include "rc.h"
#include "slices.h"
#include <ctype.h>

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
    const char *ptr = memchr(s->data, delim, s->len);
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
