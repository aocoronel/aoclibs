#ifndef AOCLIBS_CSTR_H_
#define AOCLIBS_CSTR_H_

#include "base.h"
#include <stdbool.h>
#include <stdio.h>

// NULL :: Out of memory
AOCDEF char *null cstr_dup(const char *s, const size_t len);
AOCDEF void cstr_to_lower(char *s);
AOCDEF void cstrn_to_lower(char *s, const size_t len);

AOCDEF bool cstr_begins_with(const char *restrict s,
                             const size_t s_len,
                             const char *restrict pattern,
                             size_t pattern_len);
AOCDEF bool cstr_ends_with(const char *restrict s,
                           const size_t s_len,
                           const char *restrict pattern,
                           size_t pattern_len);

#define cstr_eq(s, pattern) cstrn_eq((s), strlen((s)), (pattern), strlen((pattern)))
// Compares "s" to "pattern".
AOCDEF bool cstrn_eq(const char *restrict s,
                     const size_t s_len,
                     const char *restrict pattern,
                     size_t pattern_len);

#define cstr_eq_case(s, pattern) cstrn_eq_case((s), strlen((s)), (pattern), strlen((pattern)))
// Compares "s" to "pattern" ignoring case.
//
// Allocates a temporary variable to lowercase all characters. If the allocation fails, sets errno
// to ENOMEM.
AOCDEF bool cstrn_eq_case(const char *restrict null s,
                          const size_t s_len,
                          const char *null pattern,
                          size_t pattern_len);

// Finds if "s" has "pattern" in it.
AOCDEF bool cstr_has(const char *restrict null s,
                     size_t s_len,
                     const char *restrict null pattern,
                     size_t pattern_len);

// Finds if "s" has "pattern" in it, and returns the index, where pattern starts.
AOCDEF size_t cstr_has_at(const char *restrict null s,
                          size_t s_len,
                          const char *restrict null pattern,
                          size_t pattern_len);

// Finds if "s" has "delim" in it, and returns the index, where delim is found.
AOCDEF size_t index_of(const char *s, char delim, size_t size);

// Returns needed dest size to fit src.
//
// This is obligatory to prevent string truncation when using cstr_fmt_write.
AOCDEF int cstr_fmt_size(const char *restrict fmt, ...);
AOCDEF int cstr_fmt_write(char *restrict s, const size_t s_cap, const char *restrict fmt, ...);

AOCDEF double cstr_to_double(const char *s, const double _default);
AOCDEF bool cstr_to_bool(const char *s, const bool _default);
AOCDEF float cstr_to_float(const char *s, const float _default);
AOCDEF long cstr_to_long(const char *s, const long _default);

#ifdef AOCLIBS_IMPLEMENTATION
#include "cstr.c"
#endif // AOCLIBS_IMPLEMENTATION

#endif // AOCLIBS_CSTR_H_
