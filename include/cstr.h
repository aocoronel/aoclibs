#ifndef AOCLIBS_CSTR_H_
#define AOCLIBS_CSTR_H_

#include "base.h"
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

// Returns slice on first occurance of a word between whitespaces.
//
// Example:
//      char msg[] = "Hello, World! Hopefully, this example is useful";
//      Slice slice = extract_between_whitespace(msg, 0, sizeof(msg));
//      printf("%.*s\n", slice.len, slice.data); // => "Hello,"
//      slice = extract_between_whitespace(msg, slice.len, sizeof(msg));
//      printf("%.*s\n", slice.len, slice.data); // => "World!"
fn Slice extract_between(const char *s, size_t begin, size_t end, char delim);

#define cstr_to_slice(s, start, end)                  \
    (Slice) {                                         \
        .data = (s) + (start), .len = (end) - (start) \
    }

// Slice myslice = slice("hello, world!");
#define slice(s)                           \
    (Slice) {                              \
        .data = "" s "", .len = STRLEN(s), \
    }

fn char *null cstr_dup(const char *s, const size_t len);
fn void cstr_to_lower(char *s);
fn void cstrn_to_lower(char *s, const size_t len);

#define slice_begins_with(cs, begin, begin_len) \
    cstr_begins_with((cs)->data, (cs)->len, (begin), (begin_len))
#define slice_ends_with(cs, end, end_len) cstr_ends_with((cs)->data, (cs)->len, (end), (end_len))

fn bool cstr_begins_with(const char *restrict s,
                         const size_t s_len,
                         const char *restrict pattern,
                         size_t pattern_len);
fn bool cstr_ends_with(const char *restrict s,
                       const size_t s_len,
                       const char *restrict pattern,
                       size_t pattern_len);

#define slice_eq(cs1, cs2) cstrn_eq((cs1)->data, (cs1)->len, (cs2)->data, (cs2)->len)
#define cstr_eq(s, pattern) cstrn_eq_case((s), strlen((s)), (pattern), strlen((pattern)))
// Compares "s" to "pattern".
fn bool cstrn_eq(const char *restrict s,
                 const size_t s_len,
                 const char *restrict pattern,
                 size_t pattern_len);

#define slice_eq_case(cs1, cs2, offset) \
    cstrn_eq_case((cs1)->data + (offset), (cs1)->len, (cs2)->data, (cs2)->len)
#define cstr_eq_case(s, pattern) cstrn_eq_case((s), strlen((s)), (pattern), strlen((pattern)))
// Compares "s" to "pattern" ignoring case.
//
// Allocates a temporary variable to lowercase all characters. If the allocation fails, sets errno
// to ENOMEM.
fn bool cstrn_eq_case(const char *restrict null s,
                      const size_t s_len,
                      const char *null pattern,
                      size_t pattern_len);

// Finds if "s" has "pattern" in it.
fn bool cstr_has(const char *restrict null s,
                 size_t s_len,
                 const char *restrict null pattern,
                 size_t pattern_len);

// Finds if "s" has "pattern" in it, and returns the index, where pattern starts.
fn size_t cstr_has_at(const char *restrict null s,
                      size_t s_len,
                      const char *restrict null pattern,
                      size_t pattern_len);

// Finds if "s" has "delim" in it, and returns the index, where delim is found.
size_t index_of(const char *s, char delim, size_t size);

// Returns needed dest size to fit src.
//
// This is obligatory to prevent string truncation when using cstr_fmt_write.
fn int cstr_fmt_size(const char *restrict fmt, ...);
fn int cstr_fmt_write(char *restrict s, const size_t s_cap, const char *restrict fmt, ...);

fn double cstr_to_double(const char *s, const double _default);
fn bool cstr_to_bool(const char *s, const bool _default);
fn float cstr_to_float(const char *s, const float _default);
fn long cstr_to_long(const char *s, const long _default);

#ifdef AOCLIBS_IMPLEMENTATION
#include "cstr.c"
#endif

#endif // AOCLIBS_CSTR_H_
