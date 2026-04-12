#pragma once

#include "rc.h"
#include "slices.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#define AOCLIBS_SYMBOLS \
    X('{')              \
    X('}')              \
    X('[')              \
    X(']')              \
    X('(')              \
    X(')')              \
    X(';')              \
    X(',')              \
    X('-')              \
    X('.')              \
    X('>')              \
    X('<')              \
    X('+')              \
    X('*')              \
    X('$')              \
    X('#')              \
    X('@')              \
    X('=')              \
    X('/')              \
    X('&')              \
    X('|')              \
    X('%')              \
    X('\\')             \
    X('"')

Slices split(rc *f, char delim) {
    Slice cur = { .data = f->data, .len = f->len };
    Slices ss = { 0 };

    while (cur.len > 0) {
        const char *pos = memchr(cur.data, delim, cur.len);

        if (!pos) {
            // last segment
            da_append(&ss, cur.data, cur.len);
            break;
        }

        size_t length = pos - cur.data;

        da_append(&ss, cur.data, length);

        // move forward past delimiter
        cur.data = pos + 1;
        cur.len -= length + 1;
    }

    return ss;
}

Slice extract_between_whitespaces(const char *s, size_t begin, size_t end) {
    if (begin >= end) {
        return (Slice){ .data = NULL, .len = 0 };
    }

    size_t n_begin = begin;
    size_t n_end = end;

    while (n_begin < n_end && isspace((unsigned char)s[n_begin])) {
        n_begin++;
    }

    while (n_end > n_begin && isspace((unsigned char)s[n_end - 1])) {
        n_end--;
    }

    return (Slice){ .data = s + n_begin, .len = n_end - n_begin };
}

void slice_to_cstr(Slice s, char *buff, const size_t size) {
    int size_to_copy = s.len > size ? size : s.len;
    memcpy(buff, s.data, size_to_copy);
    buff[size_to_copy] = '\0';
}

bool while_token(Slice *cursor, int *remaining_len, Slice *out) {
    if (*remaining_len <= 0) return false;

    const char *p = cursor->data;
    size_t n = *remaining_len;

    size_t offset = 0;
    while (offset < n && p[offset] == ' ')
        offset++;

    p += offset;
    n -= offset;

    if (n == 0) {
        *remaining_len = 0;
        *out = (Slice){ .data = cursor->data - cursor->len, .len = cursor->len };
        return false;
    }

    const char *start = p;

    switch (*p) {
#define X(x) case x:
        AOCLIBS_SYMBOLS
#undef X
        cursor->data = start;
        cursor->len = 1;

        cursor->data = start;
        *remaining_len -= (offset + 1);
        cursor->data = start + 1;
        *out = (Slice){ .data = cursor->data - cursor->len, .len = cursor->len };
        return true;
    }

    size_t cursor_len = 0;
    while (cursor_len < n) {
        char c = p[cursor_len];
        if (c == ' ') break;

        switch (c) {
#define X(x) case x:
            AOCLIBS_SYMBOLS
#undef X
            goto done;
        }

        cursor_len++;
    }

done:
    cursor->data = start;
    cursor->len = cursor_len;

    *remaining_len -= (offset + cursor_len);
    cursor->data = start + cursor_len;

    *out = (Slice){ .data = cursor->data - cursor->len, .len = cursor->len };
    return cursor_len > 0;
}
