#pragma once

#include "base.h"
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

// while (begin < end) {
//     cursor = while_extract_next_word(s, &begin, end);
//     if (cursor.len == 0) break;
// }
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

Slice slice_goto_line(const Slice *s, const size_t line) {
    size_t line_count = 0;

    const char *pos = s->data;
    size_t remaining = s->len;

    while (remaining > 0) {
        const char *newline = memchr(pos, '\n', remaining);
        if (!newline) break;

        line_count++;

        if (line_count == line) {
            pos = newline + 1;
            return (Slice){ .data = pos, .len = remaining };
        }

        size_t consumed = (newline + 1) - pos;
        pos += consumed;
        remaining -= consumed;
    }

    PANIC("tried to go past the size of the slice. Slice is '%zu'"
          "lines long, but requested line '%zu'\n",
          line_count,
          line);
}

// while (while_token(&cursor, &len, &out)) {
//     printf("%.*s\n", (int)out.len, out.data);
// }
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
