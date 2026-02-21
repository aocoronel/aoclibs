#ifndef AOCLIBS_CSTR_H_
#define AOCLIBS_CSTR_H_

#include "base.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// --> printf("%s "SLICE_FMT" stuff...", "do", VSLICE(myslice));
#define SLICE_FMT "%.*s"
#define VSLICE(r) ((r).len), ((r).data)

typedef struct {
        size_t len;
        const char *data;
} cslice;

#ifndef CSTR_MALLOC
#define CSTR_MALLOC malloc
#endif

#define aoc_cstr_to_slice(s, start, end)                      \
        (cslice) {                                            \
                .data = (s) + (start), .len = (end) - (start) \
        }

// cslice myslice = lcslice("hello, world!");
#define aoc_lcslice(s)                             \
        (cslice) {                                 \
                .data = "" s "", .len = STRLEN(s), \
        }

AOCLIBS_PREFIX char *xnull aoc_cstr_dup(const char *ref s, const size_t len);
AOCLIBS_PREFIX void aoc_cstr_to_lower(char *ref s);
AOCLIBS_PREFIX void aoc_cstrn_to_lower(char *ref s, const size_t len);

#define aoc_cslice_begins_with(cs, begin, begin_len) \
        aoc_cstr_begins_with((cs)->data, (cs)->len, (begin), (begin_len))
#define aoc_cslice_ends_with(cs, end, end_len) \
        aoc_cstr_ends_with((cs)->data, (cs)->len, (end), (end_len))

AOCLIBS_PREFIX bool aoc_cstr_begins_with(const char *xref s,
                                         const size_t s_len,
                                         const char *xref pattern,
                                         size_t pattern_len);
AOCLIBS_PREFIX bool aoc_cstr_ends_with(const char *xref s,
                                       const size_t s_len,
                                       const char *xref pattern,
                                       size_t pattern_len);

#define aoc_cslice_eq(cs1, cs2) aoc_cstrn_eq((cs1)->data, (cs1)->len, (cs2)->data, (cs2)->len)
#define aoc_cstr_eq(s, pattern) aoc_cstrn_eq_case((s), strlen((s)), (pattern), strlen((pattern)))
/*
 * Compares s to pattern.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Returns if s is equal to pattern.
*/
AOCLIBS_PREFIX bool
aoc_cstrn_eq(const char *xref s, const size_t s_len, const char *xref pattern, size_t pattern_len);

#define aoc_cslice_eq_case(cs1, cs2, offset) \
        aoc_cstrn_eq_case((cs1)->data + (offset), (cs1)->len, (cs2)->data, (cs2)->len)
#define aoc_cstr_eq_case(s, pattern) \
        aoc_cstrn_eq_case((s), strlen((s)), (pattern), strlen((pattern)))
// Compares "s" to "pattern" ignoring case.
//
// Allocates a temporary variable to lowercase all characters. If the allocation fails, sets errno
// to ENOMEM.
AOCLIBS_PREFIX bool aoc_cstrn_eq_case(const char *xref s,
                                      const size_t s_len,
                                      const char *xref pattern,
                                      size_t pattern_len);

AOCLIBS_PREFIX bool
aoc_cstr_has(const char *xref s, size_t s_len, const char *xref pattern, size_t pattern_len);

/*
 * Returns needed dest size to fit src.
 *
 * This is particularly useful, when copying the contents of src to dest, to ensure the string is
 * not going to be truncated.
*/
AOCLIBS_PREFIX size_t aoc_cstrcpy_size(size_t dest_buff,
                                         const size_t dest_offset,
                                         const size_t src_len);

#define aoc_cstrcat(dest, src, dest_len, dest_cap) \
        aoc_cstrcpy((dest) + (dest_len), (src), (dest_cap))
#define aoc_cstrappend(dest, src, dest_len, dest_cap) \
        aoc_cstrcpy((dest) + (dest_len + 1), (src), (dest_cap))
/*
 * Copies src to dest, until dest_cap is reached. This function guarantees the
 * dest will be null-terminated.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Returns new length of dest
 *
 * This is a modified version of __stpncpy from the Musl LibC
*/
AOCLIBS_PREFIX size_t aoc_cstrcpy(char *xref dest, const char *xref src, size_t dest_cap);

#ifndef AOCLIBS_NO_STDIO
#include <stdarg.h>
/*
 * Returns needed dest size to fit src.
 *
 * This is obligatory to prevent string truncation when using aoc_cstrcpy_fmt.
*/
AOCLIBS_PREFIX int aoc_cstrcpy_fmt_size(const char *xref fmt, ...);
AOCLIBS_PREFIX int aoc_cstrcpy_fmt(char *xref s, const size_t s_cap, const char *xref fmt, ...);
#endif

/*
 * Finds pattern in s. Assume strings are null-terminated.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Error:
 * - SIZE_MAX :: Pattern not found
 *
 * Returns the index where pattern starts in s.
*/
AOCLIBS_PREFIX size_t aoc_cstrstr_index(const char *xref s, const char *xref pattern);

AOCLIBS_PREFIX size_t aoc_cstrnstr_index(const char *xref s,
                                         const char *xref pattern,
                                         const size_t s_len,
                                         const size_t pattern_len);

#define aoc_cstrtok(s, delim, s_len) aoc_cstr_tok((s), (#delim), 1)
#define aoc_lcstrtok(s, delim) aoc_cstr_tok((s), (#delim), STRLEN(s), 1)
#define aoc_cstrstrtok aoc_cstr_tok
AOCLIBS_PREFIX const char *null aoc_cstrstr_tok(const char *ref s,
                                                const char *delim,
                                                const size_t s_len,
                                                const size_t delim_len);

AOCLIBS_PREFIX size_t aoc_cstr_trim_whitespace(char *ref s, const size_t len);

AOCLIBS_PREFIX double aoc_cstr_to_double(const char *ref s, const double _default);
AOCLIBS_PREFIX bool aoc_cstr_to_bool(const char *ref s, const bool _default);
AOCLIBS_PREFIX float aoc_cstr_to_float(const char *ref s, const float _default);
AOCLIBS_PREFIX long aoc_cstr_to_long(const char *ref s, const long _default);

size_t aoc_index_of(const char *ref buff, char delim);
size_t aoc_index_of_till(const char *ref buff, char delim, size_t size);

#ifdef AOCLIBS_IMPLEMENTATION
#include "cstr.c"
#endif

#ifdef AOCLIBS_STRIP_PREFIX
#define lcslice aoc_lcslice
#define cstr_to_slice aoc_cstr_to_slice

#define cstrdup aoc_cstr_dup
#define cstr_to_lower aoc_cstr_to_lower
#define cstrn_to_lower aoc_cstrn_to_lower

#define cslice_eq aoc_cslice_eq
#define cslice_begins_with aoc_cslice_begins_with
#define cslice_ends_with aoc_cslice_ends_with
#define cstr_begins_with aoc_cstr_begins_with
#define cstr_ends_with aoc_cstr_ends_with
#define cstr_eq aoc_cstr_eq
#define cstrn_eq aoc_cstrn_eq
#define cstr_eq_case aoc_cstr_eq_case
#define cstrn_eq_case aoc_cstrn_eq_case

#define cstr_has aoc_cstr_has

#define cstrcpy_size aoc_cstrcpy_size

#define cstrcpy aoc_cstrcpy
#define cstrcat aoc_cstrcat
#define cstrappend aoc_cstrappend

#ifndef AOCLIBS_NO_STDIO
#define cstrcopy_fmt_size aoc_cstrcpy_fmt_size
#define cstrcopy_fmt aoc_cstrcpy_fmt
#endif

#define cstrstr_index aoc_cstrstr_index
#define cstrnstr_index aoc_cstrnstr_index

#define cstrstr_tok aoc_cstrstr_tok
#define cstrtok aoc_cstrtok
#define lcstrtok aoc_lcstrtok
#define cstrstrtok aoc_cstr_tok

#define cstr_trim_whitespace aoc_cstr_trim_whitespace

#define cstr_to_bool aoc_cstr_to_bool
#define cstr_to_double aoc_cstr_to_double
#define cstr_to_float aoc_cstr_to_float
#define cstr_to_long aoc_cstr_to_long

#define index_of aoc_index_of
#define index_of_till aoc_index_of_till
#endif

#endif // AOCLIBS_CSTR_H_
