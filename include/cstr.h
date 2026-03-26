#ifndef AOCLIBS_CSTR_H_
#define AOCLIBS_CSTR_H_

#include "base.h"
#include <stdbool.h>
#include <stdio.h>

// --> printf("%s "SLICE_FMT" stuff...", "do", VSLICE(&myslice));
#define SLICE_FMT "%.*s"
#define VSLICE(r) ((r)->len), ((r)->data)

#pragma clang assume_nonnull begin

typedef struct {
    size_t len;
    const char *data;
} Slice;

// Returns slice on first occurance of a word between whitespaces.
//
// Example:
//      char msg[] = "Hello, World! Hopefully, this example is useful";
//      Slice slice = extract_between_whitespace(msg, 0, sizeof(msg));
//      printf("%.*s\n", slice.len, slice.data); // => "Hello,"
//      slice = extract_between_whitespace(msg, slice.len, sizeof(msg));
//      printf("%.*s\n", slice.len, slice.data); // => "World!"
fn Slice aoc_extract_between(const char *s, size_t begin, size_t end, char delim);

#define aoc_cstr_to_slice(s, start, end)              \
    (Slice) {                                         \
        .data = (s) + (start), .len = (end) - (start) \
    }

// Slice myslice = slice("hello, world!");
#define aoc_slice(s)                       \
    (Slice) {                              \
        .data = "" s "", .len = STRLEN(s), \
    }

fn char *null aoc_cstr_dup(const char *s, const size_t len);
fn void aoc_cstr_to_lower(char *s);
fn void aoc_cstrn_to_lower(char *s, const size_t len);

#define aoc_slice_begins_with(cs, begin, begin_len) \
    aoc_cstr_begins_with((cs)->data, (cs)->len, (begin), (begin_len))
#define aoc_slice_ends_with(cs, end, end_len) \
    aoc_cstr_ends_with((cs)->data, (cs)->len, (end), (end_len))

fn bool aoc_cstr_begins_with(const char *restrict s,
                             const size_t s_len,
                             const char *restrict pattern,
                             size_t pattern_len);
fn bool aoc_cstr_ends_with(const char *restrict s,
                           const size_t s_len,
                           const char *restrict pattern,
                           size_t pattern_len);

#define aoc_slice_eq(cs1, cs2) aoc_cstrn_eq((cs1)->data, (cs1)->len, (cs2)->data, (cs2)->len)
#define aoc_cstr_eq(s, pattern) aoc_cstrn_eq_case((s), strlen((s)), (pattern), strlen((pattern)))
// Compares "s" to "pattern".
fn bool aoc_cstrn_eq(const char *restrict s,
                     const size_t s_len,
                     const char *restrict pattern,
                     size_t pattern_len);

#define aoc_slice_eq_case(cs1, cs2, offset) \
    aoc_cstrn_eq_case((cs1)->data + (offset), (cs1)->len, (cs2)->data, (cs2)->len)
#define aoc_cstr_eq_case(s, pattern) \
    aoc_cstrn_eq_case((s), strlen((s)), (pattern), strlen((pattern)))
// Compares "s" to "pattern" ignoring case.
//
// Allocates a temporary variable to lowercase all characters. If the allocation fails, sets errno
// to ENOMEM.
fn bool aoc_cstrn_eq_case(const char *restrict null s,
                          const size_t s_len,
                          const char *null pattern,
                          size_t pattern_len);

// Finds if "s" has "pattern" in it.
fn bool aoc_cstr_has(const char *restrict null s,
                     size_t s_len,
                     const char *restrict null pattern,
                     size_t pattern_len);

// Finds if "s" has "pattern" in it, and returns the index, where pattern starts.
fn size_t aoc_cstr_has_at(const char *restrict null s,
                          size_t s_len,
                          const char *restrict null pattern,
                          size_t pattern_len);

// Finds if "s" has "delim" in it, and returns the index, where delim is found.
size_t aoc_index_of(const char *s, char delim, size_t size);

// Returns needed size destination string needs to fit src.
//
// This is particularly useful, when copying the contents of src to dest, to ensure the string is
// not going to be truncated.
fn size_t aoc_cstrcpy_size(size_t dest_size, const size_t dest_offset, const size_t src_len);

// Copies "src" to "dest", until "dest_cap" is reached, and returns "dest" length. This function
// guarantees the dest will be null-terminated.
//
// This is a modified version of __stpncpy from the Musl LibC
//
// #AOCLIBS_CSTRCPY_AS_MEMCPY uses memcpy instead
fn size_t aoc_cstrcpy(char *restrict dest, const char *restrict src, size_t dest_cap);

// Appends "src" to "dest", until "dest_cap" is reached, and returns "dest" length. This function
// guarantees the dest will be null-terminated.
//
// Appending means the added "src" will be whitespace separated.
fn size_t aoc_cstrappend(char *restrict dest,
                         const char *restrict src,
                         size_t dest_len,
                         size_t dest_cap);

// Concatenates "src" to "dest", until "dest_cap" is reached, and returns "dest" length. This
// function guarantees the dest will be null-terminated.
fn size_t aoc_cstrcat(char *restrict dest,
                      const char *restrict src,
                      size_t dest_len,
                      size_t dest_cap);

// Returns needed dest size to fit src.
//
// This is obligatory to prevent string truncation when using aoc_cstr_fmt_write.
fn int aoc_cstr_fmt_size(const char *restrict fmt, ...);
fn int aoc_cstr_fmt_write(char *restrict s, const size_t s_cap, const char *restrict fmt, ...);

fn double aoc_cstr_to_double(const char *s, const double _default);
fn bool aoc_cstr_to_bool(const char *s, const bool _default);
fn float aoc_cstr_to_float(const char *s, const float _default);
fn long aoc_cstr_to_long(const char *s, const long _default);
#pragma clang assume_nonnull end

#ifdef AOCLIBS_IMPLEMENTATION
#include "cstr.c"
#endif

#ifdef AOCLIBS_STRIP_PREFIX
#define slice aoc_slice
#define cstr_to_slice aoc_cstr_to_slice

#define cstrdup aoc_cstr_dup
#define cstr_to_lower aoc_cstr_to_lower
#define cstrn_to_lower aoc_cstrn_to_lower

#define slice_begins_with aoc_slice_begins_with
#define slice_ends_with aoc_slice_ends_with

#define cstr_begins_with aoc_cstr_begins_with
#define cstr_ends_with aoc_cstr_ends_with

#define slice_eq aoc_slice_eq
#define cstr_eq aoc_cstr_eq
#define cstrn_eq aoc_cstrn_eq
#define cstr_eq_case aoc_cstr_eq_case
#define cstrn_eq_case aoc_cstrn_eq_case

#define cstr_has aoc_cstr_has
#define cstr_has_at aoc_cstr_has_at

#define cstrcpy_size aoc_cstrcpy_size

#define cstrcpy aoc_cstrcpy
#define cstrcat aoc_cstrcat
#define cstrappend aoc_cstrappend

#define cstr_fmt_size aoc_cstr_fmt_size
#define cstr_fmt_write aoc_cstr_fmt_write

#define cstr_to_bool aoc_cstr_to_bool
#define cstr_to_double aoc_cstr_to_double
#define cstr_to_float aoc_cstr_to_float
#define cstr_to_long aoc_cstr_to_long

#define index_of aoc_index_of
#endif

#endif // AOCLIBS_CSTR_H_
