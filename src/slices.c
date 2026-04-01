#pragma once

#include "rc.h"
#include "slices.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

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

    return (Slice){ .data = s + n_begin, .len = n_end - n_begin };
}

void slice_to_cstr(Slice s, char *buff, const size_t size) {
    int size_to_copy = s.len > size ? size : s.len;
    memcpy(buff, s.data, size_to_copy);
    buff[size_to_copy] = '\0';
}
