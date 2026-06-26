#ifndef AOC_CSTR_H_
#define AOC_CSTR_H_

#include "base.h"

// NULL :: Out of memory
AOCDEF char *null cstr_dup(const char *s, const size_t len);
AOCDEF void cstr_to_lower(char *s);
AOCDEF void cstrn_to_lower(char *s, const size_t len);
AOCDEF void cstr_to_upper(char *s);
AOCDEF void cstrn_to_upper(char *s, const size_t len);

AOCDEF bool cstr_begins_with(const char *restrict s,
							 const size_t s_len,
							 const char *restrict pattern,
							 const size_t pattern_len);
AOCDEF bool cstr_ends_with(const char *restrict s,
						   const size_t s_len,
						   const char *restrict pattern,
						   const size_t pattern_len);

// Compares "s" to "pattern".
AOCDEF bool cstr_eq(const char *restrict s, const char *restrict pattern);
// Compares "s" to "pattern".
AOCDEF bool cstrn_eq(const char *restrict s,
					 const size_t s_len,
					 const char *restrict pattern,
					 const size_t pattern_len);

// Compares "s" to "pattern" ignoring case.
//
// Allocates a temporary variable to lowercase all characters. If the allocation fails, sets errno
// to ENOMEM.
//
// 0 :: No Match
// 1 :: Match
// -1 :: Out of memory
AOCDEF char cstr_eq_case(const char *restrict s, const char *restrict pattern);

// Compares "s" to "pattern" ignoring case.
//
// Allocates a temporary variable to lowercase all characters. If the allocation fails, sets errno
// to ENOMEM.
//
// 0 :: No Match
// 1 :: Match
// -1 :: Out of memory
AOCDEF char cstrn_eq_case(const char *restrict s,
						  const size_t s_len,
						  const char *restrict pattern,
						  const size_t pattern_len);

// Finds if "s" has "pattern" in it.
// If pattern_len is 1, opt to use index_of instead
AOCDEF bool cstr_has(const char *restrict s,
					 const size_t s_len,
					 const char *restrict pattern,
					 const size_t pattern_len);

// Finds if "s" has "pattern" in it, and returns the index, where pattern starts.
AOCDEF size_t cstr_has_at(const char *restrict s,
						  const size_t s_len,
						  const char *restrict pattern,
						  const size_t pattern_len);

// Finds if "s" has "delim" in it, and returns the index, where delim is found.
AOCDEF size_t index_of(const char *s, const char delim, const size_t size);

// Returns needed dest size to fit src.
//
// This is obligatory to prevent string truncation when using cstr_fmt_write.
AOCDEF int cstr_fmt_size(const char *restrict fmt, ...);
AOCDEF int cstr_fmt_write(char *restrict s, const size_t s_cap, const char *restrict fmt, ...);

AOCDEF double cstr_to_double(const char *s, const double _default);
AOCDEF bool cstr_to_bool(const char *s, const bool _default);
AOCDEF float cstr_to_float(const char *s, const float _default);
AOCDEF long cstr_to_long(const char *s, const long _default);

// Slices

// printf("%.*s\n", $view_slice(slice))
#define $view_slice(r) ((int)(r)->len), ((r)->data)

typedef struct {
	const char *data;
	size_t len;
} Slice;

AOCDEF Slice cstr_to_slice_from(const char *str, const size_t start, const size_t end);
AOCDEF Slice cstr_to_slice(const char *str);
AOCDEF bool slice_eq(Slice lhs, Slice rhs);
AOCDEF char slice_eq_case(Slice lhs, Slice rhs);
AOCDEF bool slice_begins_with(Slice lhs, Slice rhs);
AOCDEF bool slice_begins_with_cstr(Slice lhs, const char *rhs, const size_t rhs_len);
AOCDEF bool slice_ends_with(Slice lhs, Slice rhs);
AOCDEF bool slice_ends_with_cstr(Slice lhs, const char *rhs, const size_t rhs_len);

// Slice myslice = slice("hello, world!");
#define $slice(s)                           \
	(Slice) {                               \
		.data = "" s "", .len = $strlen(s), \
	}

// while (begin < end) {
//     cursor = while_extract_next_word(s, &begin, end);
//     if (cursor.len == 0) break;
// }
AOCDEF Slice while_next_word(const char *restrict s, size_t *restrict begin, size_t end);
AOCDEF Slice while_next_word_and(const char *restrict s,
								 size_t *restrict begin,
								 size_t end,
								 char delim);

AOCDEF size_t cstr_skip_whitespace_forward(const char *pos, size_t len);
AOCDEF size_t cstr_skip_whitespace_backward(const char *pos, size_t len);
AOCDEF void slice_skip_whitespace_backward(Slice *s);
AOCDEF void slice_skip_whitespace_forward(Slice *s);
AOCDEF void slice_trim(Slice *s);
AOCDEF void slice_chop_right_by(Slice *s, char delim);
AOCDEF Slice slice_extract_from_substring(Slice *s);
AOCDEF void slice_shift_by(Slice *s, size_t len);

#ifdef AOC_IMPLEMENTATION
#include "cstr.c"
#endif // AOC_IMPLEMENTATION

#endif // AOC_CSTR_H_
