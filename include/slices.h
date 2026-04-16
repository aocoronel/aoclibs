#ifndef AOCLIBS_SLICES_H_
#define AOCLIBS_SLICES_H_

#include "base.h"
#include "rc.h"
#include <stdbool.h>
#include <stdio.h>

// printf("%.*s\n", VSLICE(slice))
#define VSLICE(r) ((int)(r)->len), ((r)->data)

typedef struct {
    size_t len;
    const char *data;
} Slice;

typedef struct {
    size_t len;
    size_t cap;
    Slice *data;
} Slices;

#define slice_eq(cs1, cs2) cstrn_eq((cs1)->data, (cs1)->len, (cs2)->data, (cs2)->len)

#define slice_eq_case(cs1, cs2, offset) \
    cstrn_eq_case((cs1)->data + (offset), (cs1)->len, (cs2)->data, (cs2)->len)

#define slice_begins_with(cs, begin, begin_len) \
    cstr_begins_with((cs)->data, (cs)->len, (begin), (begin_len))
#define slice_ends_with(cs, end, end_len) cstr_ends_with((cs)->data, (cs)->len, (end), (end_len))

#define cstr_to_slice(s, start, end)                  \
    (Slice) {                                         \
        .data = (s) + (start), .len = (end) - (start) \
    }

// Slice myslice = slice("hello, world!");
#define slice(s)                           \
    (Slice) {                              \
        .data = "" s "", .len = STRLEN(s), \
    }

AOCDEF Slices split(rc *f, char delim);

// Returns slice on first occurance of a word between whitespaces.
//
// Example:
//      char msg[] = "Hello, World! Hopefully, this example is useful";
//      Slice slice = extract_between_whitespace(msg, 0, sizeof(msg));
//      printf("%.*s\n", slice.len, slice.data); // => "Hello,"
//      slice = extract_between_whitespace(msg, slice.len, sizeof(msg));
//      printf("%.*s\n", slice.len, slice.data); // => "World!"
AOCDEF Slice extract_between_whitespaces(const char *restrict s, size_t begin, size_t end);

// while (begin < end) {
//     cursor = while_extract_next_word(s, &begin, end);
//     if (cursor.len == 0) break;
// }
AOCDEF Slice while_extract_next_word(const char *restrict s, size_t *restrict begin, size_t end);

bool while_token(Slice *cursor, int *remaining_len, Slice *out);

#ifdef AOCLIBS_IMPLEMENTATION
#include "slices.c"
#endif

#endif
